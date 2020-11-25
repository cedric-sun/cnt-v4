// cesun, 11/23/20 12:53 AM.

#ifndef CNT5106_V4_EVENT_HPP
#define CNT5106_V4_EVENT_HPP

enum class EventType {
    TimerChoke,TimerUnchoke, BcastHave, MsgChoke, MsgUnchoke,
    MsgInterest, MsgNotInterest, MsgHave, MsgRequest,
    MsgPiece
};

class Event {
public:
    const EventType event_type;

    explicit Event(const EventType event_type) : event_type(event_type) {}
};


#endif //CNT5106_V4_EVENT_HPP
