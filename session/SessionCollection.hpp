// cesun, 11/23/20 2:32 PM.

#ifndef CNT5106_V4_SESSIONCOLLECTION_HPP
#define CNT5106_V4_SESSIONCOLLECTION_HPP

#include "Session.hpp"
#include "status.hpp"

#include <vector>
#include <unordered_set>
#include <functional>

class SessionCollection {
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

    /* // a hash table impl
     * struct PnRefSet {
    private:
        // static constexpr lambda require -std=c++17 for clang++
        static constexpr auto rwsn_hasher = [](const auto &refw) { // C++14 Generic Lambda
            static std::hash<decltype(&refw.get())> hasher{};
            return hasher(&refw.get());
        };
        std::unordered_set<std::reference_wrapper<Sn>, decltype(rwsn_hasher)> pns{8, rwsn_hasher};
        using ssci = decltype(ss)::const_iterator;
    public:
        explicit PnRefSet(ssci first, ssci last) {
            while (first != last) {
                pns.emplace(**first);
                ++first;
            }
        }

        explicit PnRefSet(PnRefSet &&) = default;

        // set difference
        std::vector<std::reference_wrapper<Sn>> operator-(const PnRefSet &rhs) const {
            std::vector<std::reference_wrapper<Sn>> ret;
            ret.reserve(4);
            std::set_difference(pns.cbegin(), pns.cend(), rhs.pns.cbegin(), rhs.pns.cend(),
                                std::back_inserter(ret),
                                [](const auto &rw_a, const auto &rw_b) {
                                    return (&rw_a.get()) < (&rw_b.get());
                                });
            return ret;
        }

        void unchokeAll() {
            for (Sn &lang_ref : pns) {
                lang_ref.s.unchoke();
            }
        }
    };
     */
    struct SnRefSet {
    private:
        std::vector<std::reference_wrapper<Sn>> pns;
        using ssci = decltype(ss)::const_iterator;

        explicit SnRefSet() = default;

    public:
        explicit SnRefSet(ssci first, ssci last) {
            while (first != last) {
                pns.emplace_back(**first);
                ++first;
            }
        }

        SnRefSet(SnRefSet &&) = default;

        SnRefSet &operator=(SnRefSet &&) = default;

        // set difference
        SnRefSet operator-(const SnRefSet &rhs) const {
            SnRefSet ret;
            ret.pns.reserve(4);
            std::set_difference(pns.cbegin(), pns.cend(), rhs.pns.cbegin(), rhs.pns.cend(),
                                std::back_inserter(ret.pns),
                                [](const auto &rw_a, const auto &rw_b) {
                                    return (&rw_a.get()) < (&rw_b.get());
                                });
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

    void pnAlgorithm(int a);

    void optAlgorithm();

public:
    explicit SessionCollection(int pn_interval, int opt_interval,
                               int n_pn);

    void broadcastHave(const int i) {
        std::lock_guard lg{m};
        for (auto &e : ss) {
            e->s.ackHave(i);
        }
    }

    void newSession(Connection &&conn) {
        std::lock_guard lg{m};
        ss.emplace_back(/*TODO*/);
    }
};


#endif //CNT5106_V4_SESSIONCOLLECTION_HPP
