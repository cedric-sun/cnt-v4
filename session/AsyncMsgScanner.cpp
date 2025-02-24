// cesun, 11/25/20 3:16 PM.

#include "AsyncMsgScanner.hpp"
#include "../msg/ActualMsg.hpp"
#include "event/NonZeroMsgEvent.hpp"

#include "event/EventQueue.hpp"

#include <cxxabi.h>

void AsyncMsgScanner::scanLoop() {
    std::unique_ptr<ActualMsg> amsg_up{nullptr};
    while (true) {
        try {
            amsg_up = ActualMsg::readFrom(br);
        } catch (abi::__forced_unwind &) {
//            std::puts("scanLoop cancel exceptoin caputred.");
            throw;
        }
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
            case MsgType::TearDown:
                q.enq(std::make_unique<Event>(EventType::MsgTearDown));
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
    }
}
