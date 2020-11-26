// cesun, 11/23/20 12:44 AM.

#include "Session.hpp"
#include "../

void Session::setup() {
    HandshakeMsg{self_peer_id}.writeTo(bw);
    BitfieldMsg{self_own}.writeTo(bw);
    bw.flush();
    auto hs_msg = HandshakeMsg::readFrom(br);
    peer_id = hs_msg.peer_id;
    if (expected_peer_id != EPID_NO_PREFERENCE && peer_id != expected_peer_id)
        panic("Received handshake from an unexpected peer.");
    auto bf_msg = BitfieldMsg::readFrom(br);
    peer_own.emplace()
}

void Session::protocol() {

}
