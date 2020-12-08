// cesun, 11/27/20 12:32 AM.

#include "SessionCollection.hpp"
#include "../utils/MathUtils.hpp"
#include "../Logger.hpp"

//TODO: what if pn / opt interval is sooooo short that pn / opt is selected again before
// the selected pn / opt sessions' event queue can handle the unchoke event and change it's
// peer_choke?


#include <thread>
#include <chrono>

// welcome to javasCript++
template<typename CallBackFunc>
static void setInterval(CallBackFunc cb, const int sec) {
    //TODO ensure memory consistency
    std::thread t{[sec, cb{std::move(cb)}]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds{sec});
            cb();
        }
    }};
    t.detach();
}


SessionCollection::SessionCollection(int n_exp_session, int pn_interval, int opt_interval, int n_pn,
                                     int self_peer_id, SyncPieceBitfield &self_own,
                                     PieceRepository &repo, Logger &logger)
        : n_exp_session{n_exp_session}, n_pn{n_pn}, self_peer_id{self_peer_id}, self_own{self_own},
          repo{repo}, logger{logger} {
    setInterval([this] { pnAlgorithm(); }, pn_interval);
    setInterval([this] { optAlgorithm(); }, opt_interval);
    // TODO: issue mem fence
    gc_thread.emplace(&SessionCollection::cleanUp, this);
}

void SessionCollection::pnAlgorithm() {
    const std::lock_guard lg{m};
    for (auto &e : ss) {
        e->updateByteCount();
        e->updatePeerInterest();
        e->updateIsActive();
    }
    const int nn_pn = std::min(n_pn, static_cast<int>(ss.size()));
    // select at most nn_pn sessions that
    // 1. is Active
    // 2. is Interest in self
    // 3. downloaded the largest amount of bytes since the last PN timer tick elapse
    std::nth_element(ss.begin(), ss.begin() + nn_pn, ss.end(),
                     [](const auto &a, const auto &b) {
                         if (!a->is_active)
                             return false;
                         if (!b->is_active)
                             return true;
                         if (a->peer_interest == InterestStatus::Unknown
                             || a->peer_interest == InterestStatus::NotInterested)
                             return false;
                         if (a->peer_interest == InterestStatus::Interested &&
                             (b->peer_interest == InterestStatus::NotInterested
                              || b->peer_interest == InterestStatus::Unknown))
                             return true;
                         return (a->neo - a->old) > (b->neo - b->old);
                     });
    SnRefSet new_pn_set{};
    for (int i = 0; i < nn_pn; ++i) {
        if (ss[i]->is_active)
            new_pn_set.add(*ss[i]);
    }
    auto supplanted = pn_set - new_pn_set;
    if (!supplanted.empty()) { // promotion happened;
        supplanted.chokeAll();
        opt.reset();
    }
    // if promotion doesn't happen, nothing need to be done.
    new_pn_set.unchokeAll();
    pn_set = std::move(new_pn_set);
    logger.newPreferredNeighbors(pn_set.idVec());
}

// either absolutely don't send choke (when promotion happened during last opt interval),
// or absolute send choke to previous opt (when that does not happened during last opt interval)
void SessionCollection::optAlgorithm() {
    const std::lock_guard lg{m};
    for (auto &e : ss) {
        e->updatePeerChoke();
        e->updatePeerInterest();
        e->updateIsActive();
    }
    std::vector<std::reference_wrapper<Sn>> eligibles;
    eligibles.reserve(ss.size());
    for (auto &sn_up : ss) {
        if (sn_up->is_active && sn_up->peer_choke == ChokeStatus::Choked &&
            sn_up->peer_interest == InterestStatus::Interested)
            eligibles.emplace_back(*sn_up);
    }
    if (eligibles.empty())
        return;
    auto lucky_rw = eligibles[MathUtils::randomInt(eligibles.size())];
    lucky_rw.get().s.unchoke();
    if (opt.has_value()) { // promotion does not happen during last opt interval
        opt->get().s.choke();
    }
    // if promotion does happen, nothing need to be done.
    opt = lucky_rw;
    logger.newOptUnchokedNeighbor(opt->get().s.getPeerID());
}

void SessionCollection::tryPreempt(const Session *const s_ref) {
    const std::lock_guard lg{m};
    std::optional<std::reference_wrapper<Sn>> wrapper_sn;
    for (const auto &sn_up : ss) {
        if (&sn_up->s == s_ref) {
            wrapper_sn.emplace(*sn_up);
            break;
        }
    }
    if (!wrapper_sn.has_value())
        panic("tryPreempt() on an unknown session address");
    if (pn_set.size() < n_pn) {
        if (pn_set.contains(*wrapper_sn))
            panic("the preempting session is already a pn.");
        pn_set.add(*wrapper_sn);
        wrapper_sn->get().s.unchoke();
        return;
    }
    if (!opt.has_value()) {
        opt.emplace(*wrapper_sn);
        wrapper_sn->get().s.unchoke();
    }
}

void SessionCollection::relinquish(const Session *s_ref) {
    const std::lock_guard lg{m};
    std::optional<std::reference_wrapper<Sn>> wrapper_sn;
    for (const auto &sn_up : ss) {
        if (&sn_up->s == s_ref) {
            wrapper_sn.emplace(*sn_up);
            break;
        }
    }
    if (!wrapper_sn.has_value())
        panic("relinquish() on an unknown session address");
    if (pn_set.contains(*wrapper_sn)) {
        wrapper_sn->get().s.choke();
        pn_set.remove(*wrapper_sn);
    } else if (opt.has_value()
               && std::addressof(opt->get()) == std::addressof(wrapper_sn->get())) {
        wrapper_sn->get().s.choke();
        opt.reset();
    } else {
        return;
    }
    // an PN/OPT successfully relinquished itself; choose arbitrary choked interesting session
    for (const auto &e : ss) {
        if (e->s.isActive() && e->s.getPeerChoke() == ChokeStatus::Choked
            && e->s.getPeerInterest() == InterestStatus::Interested) {
            e->s.unchoke();
            break;
        }
    }
}

//TODO
// 1. either remove the reference to the just free-ed session here, or use weak_ptr in SnRefSet
//      to avoid dangling reference_wrapper (the relinquish should be enough to remove it...?)
// 2. downlaoded the complete file is a event of bitfield ; don't output it twice
void SessionCollection::cleanUp() {
    std::unique_lock ul{m};
    while (true) {
        cond_gc.wait(ul, [this] {
            return std::any_of(ss.cbegin(), ss.cend(), [](const auto &e) { return e->s.isDone(); });
        });
        ss.erase(std::remove_if(ss.begin(), ss.end(), [](const auto &e) {
            return e->s.isDone();
        }), ss.end());
        if (ss.empty() && n_exp_session == 0) {
            cond_end.notify_all();
            break;
        }
    }
}

void SessionCollection::wait() {
    static std::mutex m_end;
    std::unique_lock ul{m_end};
    cond_end.wait(ul, [this]() { return n_exp_session == 0; });
}
