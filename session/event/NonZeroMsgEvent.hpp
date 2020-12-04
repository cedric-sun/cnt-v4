// cesun, 11/25/20 2:49 PM.

#ifndef CNT5106_V4_NONZEROMSGEVENT_HPP
#define CNT5106_V4_NONZEROMSGEVENT_HPP

#include "Event.hpp"
#include "../../msg/msg_instantiations.hpp"
#include "../../msg/PieceMsg.hpp"
#include <memory>

template<typename MsgT, EventType ET>
class NonZeroMsgEvent : public Event {
protected:
    MsgT msg;
public:
    explicit NonZeroMsgEvent(MsgT msg) : Event{ET}, msg{std::move(msg)} {}

    virtual MsgT extract() { return std::move(msg); }
};

using HaveMsgEvent = NonZeroMsgEvent<HaveMsg, EventType::MsgHave>;
using RequestMsgEvent = NonZeroMsgEvent<RequestMsg, EventType::MsgRequest>;
using PieceMsgEvent = NonZeroMsgEvent<PieceMsg, EventType::MsgPiece>;


#endif //CNT5106_V4_NONZEROMSGEVENT_HPP
