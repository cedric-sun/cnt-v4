// cesun, 11/23/20 2:32 PM.

#ifndef CNT5106_V4_SESSIONCOLLECTION_HPP
#define CNT5106_V4_SESSIONCOLLECTION_HPP

#include "Session.hpp"
#include "status.hpp"

#include <vector>
#include <unordered_set>
#include <functional>

class SessionCollection {
public:
    DISABLE_COPY_MOVE(SessionCollection)
private:
    struct Sn {
        Session s;
        int64_t old{0}, neo{0};
        ChokeStatus peer_choke;
        InterestStatus peer_interest;

        template<typename... Args>
        explicit Sn(Args &&...args) : s{std::forward<Args>(args)...} {}

        void updateByteCount() {
            old = std::exchange(neo, s.receivedByteCount());
        }

        void updatePeerChoke() {
            peer_choke = s.getPeerChoke();
        }

        void updatePeerInterest() {
            peer_interest = s.getPeerInterest();
        }
    };

    std::vector<std::unique_ptr<Sn>> ss;

    struct SnRefSet {
    private:
        std::vector<std::reference_wrapper<Sn>> pns;
        using ssci = decltype(ss)::const_iterator;

        static constexpr auto rwcomp = [](const auto &a, const auto &b) {
            return &a.get() < &b.get();
        };

        explicit SnRefSet() = default;

    public:
        explicit SnRefSet(ssci first, ssci last) {
            while (first != last) {
                pns.emplace_back(**first);
                ++first;
            }
            std::sort(pns.begin(), pns.end(), rwcomp);
        }

        SnRefSet(SnRefSet &&) = default;

        SnRefSet &operator=(SnRefSet &&) = default;

        // set difference
        SnRefSet operator-(const SnRefSet &rhs) const {
            SnRefSet ret;
            ret.pns.reserve(4);
            std::set_difference(pns.cbegin(), pns.cend(), rhs.pns.cbegin(), rhs.pns.cend(),
                                std::back_inserter(ret.pns), rwcomp);
            return ret;
        }

        void chokeAll() {
            for (Sn &lang_ref : pns) {
                lang_ref.s.choke();
            }
        }

        void unchokeAll() {
            for (Sn &lang_ref : pns) {
                lang_ref.s.unchoke();
            }
        }

        [[nodiscard]] bool empty() const { return pns.empty(); }
    };

    std::optional<SnRefSet> pn_set{std::nullopt};
    std::optional<std::reference_wrapper<Sn>> opt{std::nullopt};
    std::mutex m;

    int self_peer_id;
    SyncPieceBitfield &self_own;
    PieceRepository &repo;
    Logger &logger;


    void pnAlgorithm(int a);

    void optAlgorithm();

public:
    explicit SessionCollection(int pn_interval, int opt_interval, int n_pn, int self_peer_id,
                               SyncPieceBitfield &self_own, PieceRepository &repo, Logger &logger);

    void broadcastHave(const int i) {
        std::lock_guard lg{m};
        for (auto &e : ss) {
            e->s.ackHave(i);
        }
    }

    void newSession(Connection &&conn, const int expected_peer_id) {
        std::lock_guard lg{m};
        ss.emplace_back(self_peer_id, expected_peer_id, std::move(conn),repo,self_own,
                        [](){},*this,logger);
    }

    void wait();
};


#endif //CNT5106_V4_SESSIONCOLLECTION_HPP
