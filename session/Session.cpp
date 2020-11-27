// cesun, 11/23/20 12:44 AM.

#include "Session.hpp"
#include "../msg/HandshakeMsg.hpp"
#include "../msg/BitfieldMsg.hpp"

void Session::setup() {
    HandshakeMsg{self_peer_id}.writeTo(bw);
    std::optional<SimplePieceBitfield> spbf;
    {
        // atomically snapshot the bitfield of self,
        // and enable queue to receive broadcast HAVE
        // TODO: ensure have broadcast is also sent in critical section
        const std::lock_guard lg{self_own};
        spbf.emplace(self_own.snapshot());
        eq.enable();
    }
    spbf->writeTo(bw);
    bw.flush();
    peer_id = HandshakeMsg::readFrom(br).peer_id;
    if (expected_peer_id != EPID_NO_PREFERENCE && peer_id != expected_peer_id)
        panic("Received handshake from an unexpected peer.");
    auto bf_msg = BitfieldMsg::readFrom(br);
    peer_own.emplace(bf_msg->extract());
    amsc.emplace(br, eq);
    amsc->start();
}

void Session::protocol() {
    setup();
    bool isDone = false;
    while (!isDone) {
        switch (auto e = eq.deq();e->event_type) {
            case EventType::TimerChoke:
                break;
            case EventType::TimerUnchoke:
                break;
            case EventType::BcastHave:
                break;
            case EventType::MsgChoke:
                break;
            case EventType::MsgUnchoke:
                break;
            case EventType::MsgInterest:
                break;
            case EventType::MsgNotInterest:
                break;
            case EventType::MsgHave:
                break;
            case EventType::MsgRequest:
                break;
            case EventType::MsgPiece:
                isDone = true;
                break;
        }
    }
    conn_up->close();
    amsc->stop();
}
