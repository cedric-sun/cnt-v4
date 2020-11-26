// cesun, 11/25/20 2:49 PM.

#ifndef CNT5106_V4_NONZEROMSGEVENT_HPP
#define CNT5106_V4_NONZEROMSGEVENT_HPP

#include "Event.hpp"
#include "../../msg/msg_instantiations.hpp"
#include "../../msg/PieceMsg.hpp"
#include <memory>

template<typename MsgT, EventType ET>
class NonZeroMsgEvent : public Event {
private:
    MsgT msg_up;
public:
    explicit NonZeroMsgEvent(MsgT msg_up) : Event{ET}, msg_up{std::move(msg_up)} {}

    MsgT extract() { return std::move(msg_up); }
};

using HaveMsgEvent = NonZeroMsgEvent<HaveMsg, EventType::MsgHave>;
using RequestMsgEvent = NonZeroMsgEvent<RequestMsg, EventType::MsgRequest>;
using PieceMsgEvent = NonZeroMsgEvent<PieceMsg, EventType::MsgPiece>;


#endif //CNT5106_V4_NONZEROMSGEVENT_HPP
