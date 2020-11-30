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

SessionCollection::SessionCollection(const int pn_interval, const int opt_interval,
                                     const int n_pn) {
    setInterval([n_pn, this] { pnAlgorithm(n_pn); }, pn_interval);
    setInterval([this] { optAlgorithm(); }, opt_interval);
}

void SessionCollection::pnAlgorithm(const int n_pn) {
    const std::lock_guard lg{m};
    for (auto &e : ss) {
        e->updateByteCount();
        e->updatePeerInterest();
    }
    const int nn_pn = std::min(n_pn, static_cast<int>(ss.size()));
    std::nth_element(ss.begin(), ss.begin() + nn_pn, ss.end(),
                     [](const auto &a, const auto &b) {
                         if (a->peer_interest == InterestStatus::Unknown
                             || a->peer_interest == InterestStatus::NotInterested)
                             return false;
                         if (a->peer_interest == InterestStatus::Interested &&
                             (b->peer_interest == InterestStatus::NotInterested
                              || b->peer_interest == InterestStatus::Unknown))
                             return true;
                         return (a->neo - a->old) < (b->neo - b->old);
                     });
    SnRefSet new_pn_set{ss.cbegin(), ss.cbegin() + nn_pn};
    if (pn_set.has_value()) {
        auto supplanted = *pn_set - new_pn_set;
        if (supplanted.empty()) { // promotion happened;
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
    }
    std::vector<std::reference_wrapper<Sn>> eligibles;
    eligibles.reserve(ss.size());
    for (auto &sn_up : ss) {
        if (sn_up->peer_choke == ChokeStatus::Choked &&
            sn_up->peer_interest == InterestStatus::Interested)
            eligibles.emplace_back(*sn_up);
    }
    auto lucky_rw = eligibles[MathUtils::randomInt(eligibles.size())];
    if (opt.has_value()) { // promotion does not happen during last opt interval
        opt->get().s.choke();
    }
    // if promotion does happen, nothing need to be done.
    opt = lucky_rw;
}
