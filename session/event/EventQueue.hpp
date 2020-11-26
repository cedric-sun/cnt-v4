// cesun, 11/23/20 12:53 AM.

#ifndef CNT5106_V4_EVENTQUEUE_HPP
#define CNT5106_V4_EVENTQUEUE_HPP

#include <memory>
#include <mutex>
#include <queue>
#include <atomic>
#include "Event.hpp"

class EventQueue {
private:
    std::queue<std::unique_ptr<Event>> q;
    std::mutex m;
    std::atomic_bool enabled = false;
public:
    EventQueue() {}

    void enable() {
        enabled = true;
    }

    void enq(std::unique_ptr<Event> e) {
        if (!enabled) return;
        const std::lock_guard lg{m};
        q.push(std::move(e));
    }

    // precond: enabled
    std::unique_ptr<Event> deq() {
        const std::lock_guard lg{m};
        auto eup = std::move(q.front());
        q.pop();
        return eup;
    }
};


#endif //CNT5106_V4_EVENTQUEUE_HPP
