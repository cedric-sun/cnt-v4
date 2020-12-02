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

    std::optional<std::thread> prot_th{std::nullopt};
    EventQueue eq{};
    std::mutex m_bcast;

    int peer_id;
    std::optional<PieceBitfield> peer_own;
    std::optional<AsyncMsgScanner> amsc;

    ChokeStatus self_choke{ChokeStatus::Unknown}; // whether self is choked by peer

    // whether peer is choked by self
    std::atomic<ChokeStatus> peer_choke{ChokeStatus::Unknown};

    // whether peer is interested in self
    std::atomic<InterestStatus> peer_interest{InterestStatus::Unknown};

    // whether self is interested in peer
    InterestStatus self_interest{InterestStatus::Unknown};

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
        // see either
        //      snapshot is taken and queue is enabled, or
        //      snapshot is not taken and queue is disabled
        const std::lock_guard lg{m_bcast};
        eq.enq(std::make_unique<BcastHaveEvent>(i));
    }

    void choke() {
        eq.enq(std::make_unique<Event>(EventType::TimerChoke));
    }

    void unchoke() {
        eq.enq(std::make_unique<Event>(EventType::TimerUnchoke));
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
