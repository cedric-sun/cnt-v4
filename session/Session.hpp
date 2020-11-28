// cesun, 11/23/20 12:44 AM.

#ifndef CNT5106_V4_SESSION_HPP
#define CNT5106_V4_SESSION_HPP

#include "event/EventQueue.hpp"
#include "event/BcastHaveEvent.hpp"
#include "../tcp/Connection.hpp"
#include "../io/BufferedWriter.hpp"
#include "../io/BufferedReader.hpp"
#include "../piecebitfield/SimplePieceBitfield.hpp"
#include "AsyncMsgScanner.hpp"
#include "status.hpp"
#include <memory>
#include <thread>

class SyncPieceBitfield;

class Session {
public:
    static const int EPID_NO_PREFERENCE = -1;
private:
    int peer_id;
    const int self_peer_id, expected_peer_id;
    std::unique_ptr<Connection> conn_up;
    std::optional<std::thread> prot_th;
    BufferedReader br;
    BufferedWriter bw;

    std::optional<SimplePieceBitfield> peer_own;
    SyncPieceBitfield &self_own;

    EventQueue eq;
    std::optional<AsyncMsgScanner> amsc;

    ChokeStatus self_choke{ChokeStatus::Unknown}; // whether self is choked by peer

    // whether peer is choked by self
    std::atomic<ChokeStatus> peer_choke{ChokeStatus::Unknown};

    // whether peer is interested in self
    std::atomic<InterestStatus> peer_interest{InterestStatus::Unknown};

    void setup();

    void protocol();

public:
    Session(const int self_peer_id, const int expected_peer_id,
            std::unique_ptr<Connection> conn_up, SyncPieceBitfield &self_own)
            : self_peer_id{self_peer_id}, expected_peer_id{expected_peer_id},
              conn_up{std::move(conn_up)}, br{*this->conn_up}, bw{*this->conn_up},
              self_own{self_own}{
    }

    void start() {
        prot_th.emplace(&Session::protocol, this);
    }

    void ackHave(const int i) {
        eq.enq(std::make_unique<BcastHaveEvent>(i));
    }

    void choke() {
        eq.enq(std::make_unique<Event>(EventType::TimerChoke));
    }

    void unchoke() {
        eq.enq(std::make_unique<Event>(EventType::TimerUnchoke));
    }

    [[nodiscard]] int64_t receivedByteCount() const {
        return conn_up->receivedByteCount();
    }

    [[nodiscard]] ChokeStatus getPeerChoke() const {
        return static_cast<ChokeStatus>(peer_choke);
    }

    [[nodiscard]] InterestStatus getPeerInterest() const {
        return static_cast<InterestStatus>(peer_interest);
    }
};


#endif //CNT5106_V4_SESSION_HPP
