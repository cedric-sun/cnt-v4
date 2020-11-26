// cesun, 11/25/20 3:16 PM.

#include "AsyncMsgScanner.hpp"
#include "../msg/msg/ActualMsg.hpp"
#include "event/NonZeroMsgEvent.hpp"
#include "../utils/uptr_cast.hpp"

void AsyncMsgScanner::scanLoop() {
    while (true) {
        auto amsg_up = ActualMsg::readFrom(br);
        switch (amsg_up->type) {
            case MsgType::Choke:
                q.enq(std::make_unique<Event>(EventType::MsgChoke));
                break;
            case MsgType::Unchoke:
                q.enq(std::make_unique<Event>(EventType::MsgUnchoke));
                break;
            case MsgType::Interested:
                q.enq(std::make_unique<Event>(EventType::MsgInterest));
                break;
            case MsgType::NotInterested:
                q.enq(std::make_unique<Event>(EventType::MsgNotInterest));
                break;
            case MsgType::Have: {
                auto have_msg = std::move(*static_cast<HaveMsg *>(amsg_up.get()));
                q.enq(std::make_unique<HaveMsgEvent>(std::move(have_msg)));
            }
                break;
            case MsgType::Bitfield:
                panic("Receive unexpected Bitfield message");
                break;
            case MsgType::Request: {
                auto request_msg = std::move(*static_cast<RequestMsg *>(amsg_up.get()));
                q.enq(std::make_unique<RequestMsgEvent>(std::move(request_msg)));
            }
                break;
            case MsgType::Piece: {
                auto piece_msg = std::move(*static_cast<PieceMsg *>(amsg_up.get()));
                q.enq(std::make_unique<PieceMsgEvent>(std::move(piece_msg)));
            }
                break;
            default:
                panic("Message of unknown type is received.");
        }
        break;
    }
}
