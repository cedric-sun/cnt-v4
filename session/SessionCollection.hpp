// cesun, 11/23/20 2:32 PM.

#ifndef CNT5106_V4_SESSIONCOLLECTION_HPP
#define CNT5106_V4_SESSIONCOLLECTION_HPP

#include "Session.hpp"
#include "status.hpp"

#include <vector>

class SessionCollection {
private:
    struct Sn {
        std::unique_ptr<Session> s;
        int64_t old{0}, neo{0};
        ChokeStatus peer_choke;
        InterestStatus peer_interest;

        explicit Sn(std::unique_ptr<Session> s) : s{std::move(s)} {}

        void updateByteCount() {
            old = std::exchange(neo, s->receivedByteCount());
        }

        void updatePeerChoke() {
            peer_choke = s->getPeerChoke();
        }

        void updatePeerInterest() {
            peer_interest = s->getPeerInterest();
        }
    };

    std::vector<Sn> ss;
    std::mutex m;

    void pn_algorithm(int n_pn);

    void opt_algorithm();
public:
    explicit SessionCollection(int pn_interval, int opt_interval,
                               int n_pn);

    void broadcastHave(const int i) {
        std::lock_guard lg{m};
        std::for_each(ss.begin(), ss.end(), [i](auto &s) { s->ackHave(i); });
    }

    void newSession(Connection &&conn) {
        auto s = std::make_unique<Session>(/*TODO*/);
        std::lock_guard lg{m};
        ss.emplace_back(std::move(s));
    }
};


#endif //CNT5106_V4_SESSIONCOLLECTION_HPP
