// cesun, 11/23/20 12:44 AM.

#include "Session.hpp"
#include "../msg/HandshakeMsg.hpp"
#include "../msg/BitfieldMsg.hpp"
#include "../piecebitfield/SyncPieceBitfield.hpp"
#include "../msg/msg_instantiations.hpp"
#include "event/NonZeroMsgEvent.hpp"
#include "../utils/MathUtils.hpp"
#include "../storage/PieceRepository.hpp"
#include "SessionCollection.hpp"
#include "../Logger.hpp"

void Session::setup() {
    {// sending
        HandshakeMsg{self_peer_id}.writeTo(bw);
        std::optional<BitfieldMsg> bf_msg;
        {
            const std::lock_guard lg{m_bcast};
            bf_msg.emplace(self_own.snapshot());
            is_bcast_ready = true;
        }
        bf_msg->writeTo(bw);
        bw.flush();
    }
    {//receiving
        peer_id = HandshakeMsg::readFrom(br).peer_id;
        if (expected_peer_id != EPID_NO_PREFERENCE) { // self is client
            if (peer_id != expected_peer_id) {
                panic("Received handshake from an unexpected peer.");
            }
            logger.selfConnectedTo(peer_id);
        } else { // self is server
            logger.selfConnectedBy(peer_id);
        }

        auto bf_msg = BitfieldMsg::readFrom(br);
        peer_own.emplace(bf_msg->extract());
        if (peer_own->owningAll() && self_own.owningAll()) {
            is_done = true;
            return;
        }
    }
    if ((*peer_own - self_own).empty()) {
        NotInterestedMsg{}.writeTo(bw);
    } else {
        InterestedMsg{}.writeTo(bw);
    }
    bw.flush();
    amsc.emplace(br, eq);
    amsc->start();
    is_active = true;
}

// precondition: self_choke == ChokeStatus::Unchoked
void Session::requestNextIfPossible() {
    //Acquiring all eligible indexes + randomly select i + setRequest(i) needs to be atomic
    // as a whole
    static std::mutex m_req; // shared across ALL sessions
    int i = -1;
    {
        const std::lock_guard lg{m_req};
        auto eligible_indexes = *peer_own - self_own;
        if (!eligible_indexes.empty()) {
            i = eligible_indexes[MathUtils::randomInt(eligible_indexes.size())];
            self_own.setRequested(i);
        } else {
            // peer unchoked self because it thinks that self is interested in it,
            // but now we found self is actually not interested in peer.
            // One possible thing is that self lost interest in peer in a broadcast have
            // event, but peer made the decision of unchoking self before it can update its
            // state of whether self is interested in it.
            // We just ignore such unchoke for now.
        }
    }
    if (i != -1) {
        RequestMsg{i}.writeTo(bw);
        bw.flush();
    }
}

void Session::protocol() {
    setup();
    while (!is_done) {
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
                if ((*peer_own - self_own).empty()) {
                    NotInterestedMsg{}.writeTo(bw);
                }
                bw.flush();
                if (self_own.owningAll() && peer_own->owningAll()) {
                    is_done = true;
                    break;
                }
                break;
            case EventType::MsgChoke:
                self_choke = ChokeStatus::Choked;
                logger.chokeReceived(peer_id);
                break;
            case EventType::MsgUnchoke:
                self_choke = ChokeStatus::Unchoked;
                logger.unchokeReceived(peer_id);
                requestNextIfPossible();
                break;
            case EventType::MsgInterest:
                peer_interest = InterestStatus::Interested;
                logger.interestedReceived(peer_id);
                sc.tryPreempt(this);
                break;
            case EventType::MsgNotInterest:
                peer_interest = InterestStatus::NotInterested;
                logger.NotInterestedReceived(peer_id);
                if (peer_choke == ChokeStatus::Unchoked) {
                    // peer lost interest in self while it's unchoked;
                    // choke it immediately and try accommodate other session
                    sc.relinquish(this);
                }
                ChokeMsg{}.writeTo(bw);
                bw.flush();
                break;
            case EventType::MsgHave: {
                auto have_msg = static_cast<HaveMsgEvent *>(e.get())->extract();
                logger.haveReceived(peer_id, have_msg.piece_id);
                if (peer_own->isOwned(have_msg.piece_id))
                    panic("peer send HAVE for an piece index that self think peer already owned");
                peer_own->setOwned(have_msg.piece_id);
                if (self_own.owningAll() && peer_own->owningAll()) {
                    is_done = true;
                    break;
                }
                if (!(*peer_own - self_own).empty()) {
                    InterestedMsg{}.writeTo(bw);
                    bw.flush();
                }
            }
                break;
            case EventType::MsgRequest: {
                auto req_msg = static_cast<RequestMsgEvent *>(e.get())->extract();
                if (!self_own.isOwned(req_msg.piece_id))
                    panic("peer is requesting a piece self doesn't own");
                PieceMsg{req_msg.piece_id, repo.get(req_msg.piece_id)}.writeTo(bw);
                bw.flush();
            }
                break;
            case EventType::MsgPiece:
                auto piece_msg = static_cast<PieceMsgEvent *>(e.get())->extract();
                if (!self_own.isRequested(piece_msg.piece_id))
                    panic("received a piece that self didn't request");
                repo.save(piece_msg.piece_id, piece_msg.getPiece()); // moved
                // setOwned() after pieceDownloaded() prevents fileDownloaded() log entry appearing
                // before pieceDownloaded() of the last piece;
                logger.pieceDownloaded(peer_id, piece_msg.piece_id,
                                       self_own.numOwned() + 1);
                self_own.setOwned(piece_msg.piece_id);
                sc.broadcastHave(piece_msg.piece_id);
                if (self_choke == ChokeStatus::Unchoked) {
                    requestNextIfPossible();
                }
                break;
        }
    }
    sc.relinquish(this);
    if (amsc.has_value())
        amsc->stop();
    end_cb();
}
