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
        bool is_active;

        template<typename... Args>
        explicit Sn(Args &&...args) : s{std::forward<Args>(args)...} {}

        void updateIsActive() {
            is_active = s.isActive();
        }

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

    struct SnRefSet {
    private:
        std::vector<std::reference_wrapper<Sn>> pns;

        static constexpr auto addr_comp = [](const auto &a, const auto &b) {
            return &a.get() < &b.get();
        };
    public:
        SnRefSet() = default;

        SnRefSet(SnRefSet &&) = default;

        SnRefSet &operator=(SnRefSet &&) = default;

        DISABLE_COPY(SnRefSet)

        [[nodiscard]] int size() const {
            return pns.size();
        }

        bool contains(std::reference_wrapper<Sn> sn_ref) {
            auto it = std::lower_bound(pns.cbegin(), pns.cend(), sn_ref, addr_comp);
            return it != pns.cend() &&
                   std::addressof(it->get()) == std::addressof(sn_ref.get());
        }

        // precondition: contains(sn) == true
        void remove(std::reference_wrapper<Sn> sn_ref) {
            auto it = std::lower_bound(pns.cbegin(), pns.cend(), sn_ref, addr_comp);
            pns.erase(it);
        }

        void add(std::reference_wrapper<Sn> sn_ref) {
            auto it = std::lower_bound(pns.cbegin(), pns.cend(), sn_ref, addr_comp);
            pns.insert(it, sn_ref);
        }

        // set difference
        SnRefSet operator-(const SnRefSet &rhs) const {
            SnRefSet ret;
            ret.pns.reserve(4);
            std::set_difference(pns.cbegin(), pns.cend(), rhs.pns.cbegin(), rhs.pns.cend(),
                                std::back_inserter(ret.pns), addr_comp);
            return ret;
        }

        void chokeAll() {
            for (Sn &sn : pns) {
                sn.s.choke();
            }
        }

        void unchokeAll() {
            for (Sn &sn : pns) {
                sn.s.unchoke();
            }
        }

        [[nodiscard]] bool empty() const { return pns.empty(); }

        [[nodiscard]] std::vector<int> idVec() const {
            std::vector<int> ret;
            std::transform(pns.cbegin(), pns.cend(), std::back_inserter(ret),
                           [](const Sn &sn) { return sn.s.getPeerID(); });
            return ret;
        }
    };

    std::vector<std::unique_ptr<Sn>> ss;
    SnRefSet pn_set{};
    std::optional<std::reference_wrapper<Sn>> opt{std::nullopt};
    std::mutex m;

    int n_unfinished_session;
    const int n_pn;
    int self_peer_id;
    SyncPieceBitfield &self_own;
    PieceRepository &repo;
    Logger &logger;

    BlockingQueue<const Session *> gc_bq;
    std::condition_variable cond_end;
    std::optional<std::jthread> gc_thread;


    void pnAlgorithm();

    void optAlgorithm();

    void cleanUp();

public:
    explicit SessionCollection(int n_unfinished_session, int pn_interval, int opt_interval,
                               int n_pn, int self_peer_id, SyncPieceBitfield &self_own,
                               PieceRepository &repo, Logger &logger);

    void broadcastHave(const int i) {
        std::lock_guard lg{m};
        for (auto &e : ss) {
            e->s.ackHave(i);
        }
    }

    void tryPreempt(const Session *);

    void relinquish(const Session *);

    void notifyCleanUp(const Session *);

    void newSession(Connection &&conn, const int expected_peer_id) {
        std::lock_guard lg{m};
        auto sn_up = std::make_unique<Sn>(self_peer_id, expected_peer_id, std::move(conn), repo,
                                          self_own, *this, logger);
        ss.push_back(std::move(sn_up));
        ss.back()->s.start();
    }

    void wait();
};


#endif //CNT5106_V4_SESSIONCOLLECTION_HPP
