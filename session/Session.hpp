// cesun, 11/23/20 12:44 AM.

#ifndef CNT5106_V4_SESSION_HPP
#define CNT5106_V4_SESSION_HPP

#include "event/EventQueue.hpp"
#include "event/BcastHaveEvent.hpp"
#include "../tcp/Connection.hpp"
#include "../io/BufferedWriter.hpp"
#include "../io/BufferedReader.hpp"
#include "../piecebitfield/PieceBitfield.hpp"
#include "AsyncMsgScanner.hpp"
#include "status.hpp"
#include <memory>
#include <thread>
#include <functional>
#include <utility>

class SyncPieceBitfield;

class Logger;

class PieceRepository;

class SessionCollection;

class Session {
public:
    DISABLE_COPY_MOVE(Session)

    static const int EPID_NO_PREFERENCE = -1;
private:
    const int self_peer_id, expected_peer_id;
    Connection conn;
    BufferedReader br;
    BufferedWriter bw;
    PieceRepository &repo;
    SyncPieceBitfield &self_own;
    std::function<void(void)> end_cb;
    SessionCollection &sc;
    Logger &logger;

    std::optional<std::jthread> prot_th{std::nullopt};

    EventQueue eq{};
    std::mutex m_bcast;
    std::atomic_bool is_active{false};
    std::atomic_bool is_bcast_ready{false};
    std::atomic_bool is_done{false};

    std::atomic_int peer_id;
    std::optional<PieceBitfield> peer_own;
    std::optional<AsyncMsgScanner> amsc;

    // whether self is choked by peer
    ChokeStatus self_choke{ChokeStatus::Unknown};

    // whether peer is choked by self
    std::atomic<ChokeStatus> peer_choke{ChokeStatus::Unknown};

    // whether peer is interested in self
    std::atomic<InterestStatus> peer_interest{InterestStatus::Unknown};

    // whether self is interested in peer
    //InterestStatus self_interest{InterestStatus::Unknown};
    // the problem with this data member is that, essentially it serves as a cached result
    // of whether (*peer_own - self_own) is empty; But since self_own always changes out of
    // the control of a single protocol thread, there is no way for such cache to be accurate.
    // Its being Interested can be false positive (i.e. even we tested that (self_interest ==
    // InterestStatus::Interested) holds, an immediate (*peer_own - self_own) can still produce an
    // empty index vector).
    // Its being NotInterested is always honest. This is because since such
    // `self_interest = NotInterested` is set previously in some event handler in the same thread,
    // the only thing that can happened to `self_own` is that an ABSENT piece slot become REQUESTED
    // or OWNED; And none of these two type of changes could affect the result of
    // (*peer_own - self_own).

    void setup();

    void protocol();

    void requestNextIfPossible();

public:
    explicit Session(const int self_peer_id, const int expected_peer_id,
                     Connection &&conn, PieceRepository &repo, SyncPieceBitfield &self_own,
                     std::function<void(void)> end_cb, SessionCollection &sc, Logger &logger)
            : self_peer_id{self_peer_id}, expected_peer_id{expected_peer_id}, conn{std::move(conn)},
              br{conn}, bw{conn}, repo{repo}, self_own{self_own}, end_cb{std::move(end_cb)},
              sc{sc}, logger{logger} {
    }

    void start() { // TODO: start after ctor is still not thread safe... issue memory barrier here
        prot_th.emplace(&Session::protocol, this);
    }

    [[nodiscard]] int getPeerID() const {
        return peer_id;
    }

    // 1. We want the SessionCollection to create Session object
    //    a) callback
    //    b) bundled dependencies
    //2. The EventQueue is a real object data member of Session
    //    a) simplified the design
    //    b) no obvious reason to lazy initialize it
    //
    //Then: once the emplace_back of a new unique_ptr to Sn which embeds the Session
    // happens, this Session (i.e. its event queue) is exposed to
    //    1. PN timer
    //        When very few session is currently installed, it's possible that
    //        one tick of the PN timer choose a session that's just installed
    //        (handshake/bitfield undone), before the event loop has any chance to
    //          do immediate accommodation.
    //
    //    2. OPT timer
    //        Is not a problem, since it only pick Choked peer; before the very
    //        first peer_choke is set to Choked/Unchoked (which is when the setup
    //        Interest/NotInterest is received), it is Unknown;
    //    3. the BCAST.
    //        Before the snapshot is taken, we don't care about any bcast
    //        After the snapshot is taken, we don't wanna miss any bcast
    //
    void ackHave(const int i) {
        if (is_bcast_ready) {
            eq.enq(std::make_unique<BcastHaveEvent>(i));
        } else {
            const std::lock_guard lg{m_bcast};
            if (is_bcast_ready)
                eq.enq(std::make_unique<BcastHaveEvent>(i));
        }
    }

    void choke() {
        if (is_active)
            eq.enq(std::make_unique<Event>(EventType::TimerChoke));
    }

    void unchoke() {
        if (is_active)
            eq.enq(std::make_unique<Event>(EventType::TimerUnchoke));
    }

    [[nodiscard]] bool isDone() const {
        return is_done;
    }

    [[nodiscard]] bool isActive() const {
        return is_active;
    }

    // thread-safe
    [[nodiscard]] int64_t receivedByteCount() const {
        return conn.receivedByteCount();
    }

    // thread-safe
    [[nodiscard]] ChokeStatus getPeerChoke() const {
        return static_cast<ChokeStatus>(peer_choke);
    }

    // thread-safe
    [[nodiscard]] InterestStatus getPeerInterest() const {
        return static_cast<InterestStatus>(peer_interest);
    }
};


#endif //CNT5106_V4_SESSION_HPP
