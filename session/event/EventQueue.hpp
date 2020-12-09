// cesun, 11/23/20 12:53 AM.

#ifndef CNT5106_V4_EVENTQUEUE_HPP
#define CNT5106_V4_EVENTQUEUE_HPP

#include <memory>
#include "../../BlockingQueue.hpp"
#include "Event.hpp"

using EventQueue = BlockingQueue<std::unique_ptr<Event>>;


#endif //CNT5106_V4_EVENTQUEUE_HPP
