// cesun, 11/23/20 12:44 AM.

#ifndef CNT5106_V4_SESSION_HPP
#define CNT5106_V4_SESSION_HPP

#include "event/EventQueue.hpp"
#include "../tcp/Connection.hpp"
#include "../io/BufferedWriter.hpp"
#include "../io/BufferedReader.hpp"
#include "../PieceBitfield.hpp"
#include <memory>
#include <thread>


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

    std::optional<PieceBitfield> peer_own;
    PieceBitfield &self_own;

    EventQueue eq;

    void setup();

    void protocol();

public:
    Session(const int self_peer_id, const int expected_peer_id,
            std::unique_ptr<Connection> conn_up)
            : self_peer_id{self_peer_id}, expected_peer_id{expected_peer_id},
              conn_up{std::move(conn_up)}, msg_factory{msg_factory} {
    }

    void start() {
        prot_th.emplace(&Session::protocol, this);
    }

    void ackHave(const int i) {
        eq.enq(std::make_unique<Event>(EventType::BcastHave));
    }

    void choke() {
        eq.enq(std::make_unique<Event>(EventType::TimerChoke));
    }

    void unchoke() {
        eq.enq(std::make_unique<Event>(EventType::TimerUnchoke));
    }
};


#endif //CNT5106_V4_SESSION_HPP
