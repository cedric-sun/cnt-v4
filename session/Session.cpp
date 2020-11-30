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
                bw.flush();
                break;
            case EventType::MsgChoke:
                self_choke = ChokeStatus::Choked;
                break;
            case EventType::MsgUnchoke:
                self_choke = ChokeStatus::Unchoked;
                {
                    int i;
                    {
                        const std::lock_guard lg{self_own};
                        auto eligible_indexes = *peer_own - self_own;
                        i = eligible_indexes[MathUtils::randomInt(eligible_indexes.size())];
                        self_own[i] = PieceStatus::REQUESTED;
                    }
                    RequestMsg{i}.writeTo(bw);
                    bw.flush();
                }
                break;
            case EventType::MsgInterest:
                peer_interest = InterestStatus::Interested;
                break;
            case EventType::MsgNotInterest:
                peer_interest = InterestStatus::NotInterested;
                break;
            case EventType::MsgHave: {
                auto have_msg = static_cast<HaveMsgEvent *>(e.get())->extract();
                peer_own->setOwned(have_msg.piece_id);
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
                    //TODO
                }
                break;
        }
    }
    conn_up->close();
    amsc->stop();
    end_cb();
}
