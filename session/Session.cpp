// cesun, 11/23/20 12:44 AM.

#include "Session.hpp"
#include "../msg/HandshakeMsg.hpp"
#include "../msg/BitfieldMsg.hpp"
#include "../piecebitfield/SyncPieceBitfield.hpp"
#include "../msg/msg_instantiations.hpp"
#include "event/NonZeroMsgEvent.hpp"
#include "../utils/MathUtils.hpp"
#include "../storage/PieceRepository.hpp"

void Session::setup() {
    HandshakeMsg{self_peer_id}.writeTo(bw);
    std::optional<PieceBitfieldSnapshot> snap;
    {
        // atomically snapshot the bitfield of self,
        // and enable queue to receive broadcast HAVE
        // TODO: ensure have broadcast is also sent in critical section
        snap.emplace(self_own.snapshot());
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
                if (peer_choke != ChokeStatus::Choked) {
                    peer_choke = ChokeStatus::Choked;
                    ChokeMsg{}.writeTo(bw);
                    bw.flush();
                }
                break;
            case EventType::TimerUnchoke:
                if (peer_choke != ChokeStatus::Unchoked) {
                    peer_choke = ChokeStatus::Unchoked;
                    UnchokeMsg{}.writeTo(bw);
                    bw.flush();
                }
                break;
            case EventType::BcastHave:
                HaveMsg{static_cast<BcastHaveEvent *>(e.get())->piece_id}.writeTo(bw);
                if (self_interest == InterestStatus::Interested && (*peer_own - self_own).empty()) {
                    self_interest = InterestStatus::NotInterested;
                    NotInterestedMsg{}.writeTo(bw);
                }
                bw.flush();
                break;
            case EventType::MsgChoke:
                self_choke = ChokeStatus::Choked;
                break;
            case EventType::MsgUnchoke:
                self_choke = ChokeStatus::Unchoked;
                if (self_interest == InterestStatus::Interested) {
                    int i;
                    {
                        const std::lock_guard lg{self_own};
                        auto eligible_indexes = *peer_own - self_own;
                        if (eligible_indexes.empty())
                            panic("contradictory self_interest and bitfield diff");
                        i = eligible_indexes[MathUtils::randomInt(eligible_indexes.size())];
                        self_own[i] = PieceStatus::REQUESTED;
                    }
                    RequestMsg{i}.writeTo(bw);
                    bw.flush();
                } else {
                    // peer unchoked self because it thinks that self is interested in it,
                    // but now we found self is actually not interested in peer.
                    // One possible thing is that self lost interest in peer in a broadcast have
                    // event, but peer made the decision on unchoke self before it can update its
                    // state of whether self is interested in it.
                    // We just ignore such unchoke for now.
                }
                break;
            case EventType::MsgInterest:
                peer_interest = InterestStatus::Interested;
                // TODO: try accomodate peer immediately
                break;
            case EventType::MsgNotInterest:
                peer_interest = InterestStatus::NotInterested;
                if (peer_choke == ChokeStatus::Unchoked) {
                    // TODO: peer lost interest in self while it's unchoked;
                    //      choke it immediately and try accomodate other session
                }
                break;
            case EventType::MsgHave: {
                auto have_msg = static_cast<HaveMsgEvent *>(e.get())->extract();
                peer_own->setOwned(have_msg.piece_id);
                if (self_own.owningAll() && peer_own->owningAll()) {
                    isDone = true;
                    break;
                }
                if (!(*peer_own - self_own).empty()) {
                    self_interest = true;
                    InterestedMsg{}.writeTo(bw);
                    bw.flush();
                }
            }
                break;
            case EventType::MsgRequest: {
                auto req_msg = static_cast<RequestMsgEvent *>(e.get())->extract();
                PieceMsg{req_msg.piece_id, repo.get(req_msg.piece_id)}.writeTo(bw);
                bw.flush();
            }
                break;
            case EventType::MsgPiece:
                auto piece_msg = static_cast<PieceMsgEvent *>(e.get())->extract();
                repo.save(piece_msg.piece_id, piece_msg.getPiece());
                if (self_own.owningAll() && peer_own->owningAll()) {
                    isDone = true;
                    break;
                }
                if (self_choke == ChokeStatus::Unchoked) { // continue to request next...

                }
                break;
        }
    }
    conn_up->close();
    amsc->stop();
    end_cb();
}
