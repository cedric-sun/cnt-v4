// cesun, 11/27/20 11:05 PM.

#ifndef CNT5106_V4_BCASTHAVEEVENT_HPP
#define CNT5106_V4_BCASTHAVEEVENT_HPP

#include "Event.hpp"

class BcastHaveEvent : public Event {
public:
    const int piece_id;

    BcastHaveEvent(const int piece_id) : Event{EventType::BcastHave}, piece_id{piece_id} {}
};


#endif //CNT5106_V4_BCASTHAVEEVENT_HPP
