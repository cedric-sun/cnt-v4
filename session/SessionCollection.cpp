// cesun, 11/27/20 12:32 AM.

#include "SessionCollection.hpp"
#include "../utils/MathUtils.hpp"

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


SessionCollection::SessionCollection(int pn_interval, int opt_interval, int n_pn, int self_peer_id,
                                     SyncPieceBitfield &self_own, PieceRepository &repo,
                                     Logger &logger)
        : self_peer_id{self_peer_id}, self_own{self_own}, repo{repo},
          logger{logger} {
    setInterval([n_pn, this] { pnAlgorithm(n_pn); }, pn_interval);
    setInterval([this] { optAlgorithm(); }, opt_interval);
}

void SessionCollection::pnAlgorithm(const int n_pn) {
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
    if (pn_set.has_value()) {
        auto supplanted = *pn_set - new_pn_set;
        if (!supplanted.empty()) { // promotion happened;
            supplanted.chokeAll();
            opt.reset();
        }
        // if promotion doesn't happen, nothing need to be done.
    }
    new_pn_set.unchokeAll();
    pn_set = std::move(new_pn_set);
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
}
