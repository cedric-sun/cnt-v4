// cesun, 11/27/20 12:32 AM.

#include "SessionCollection.hpp"
#include <random>

//TODO: what if pn / opt interval is sooooo short that pn / opt is selected again before
// the selected pn / opt sessions' event queue can handle the unchoke event and change it's
// peer_choke?

static int randomInt(const int upper) {
    static std::default_random_engine rng{std::random_device{}()};
    static std::uniform_int_distribution uniform{};
    return uniform(rng) % upper;
}
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
    setInterval([n_pn, this] { pn_algorithm(n_pn); }, pn_interval);
    setInterval([this] { opt_algorithm(); }, opt_interval);
}

void SessionCollection::pn_algorithm(const int n_pn) {
    const std::lock_guard lg{m};
    for (auto &e : ss) {
        e.updateByteCount();
        e.updatePeerInterest();
    }
    std::nth_element(ss.begin(), ss.begin() + n_pn, ss.end(),
                     [](const auto &sn0, const auto &sn1) {
                         if (sn0.peer_interest == InterestStatus::Unknown
                             || sn0.peer_interest == InterestStatus::NotInterested)
                             return false;
                         if (sn0.peer_interest == InterestStatus::Interested &&
                             (sn1.peer_interest == InterestStatus::NotInterested
                              || sn1.peer_interest == InterestStatus::Unknown))
                             return true;
                         return (sn0.neo - sn0.old) < (sn1.neo - sn1.old);
                     });
    std::for_each_n(ss.begin(), n_pn, [](auto &e) { e.s->unchoke(); });
}

void SessionCollection::opt_algorithm() {
    const std::lock_guard lg{m};
    for (auto &e : ss) {
        e.updatePeerChoke();
        e.updatePeerInterest();
    }
    std::vector<Sn *> eligibles;
    eligibles.reserve(ss.size());
    for (auto &e : ss) {
        if (e.peer_choke == ChokeStatus::Choked &&
            e.peer_interest == InterestStatus::Interested)
            eligibles.push_back(&e);
    }
    eligibles[randomInt(eligibles.size())]->s->unchoke();
}
