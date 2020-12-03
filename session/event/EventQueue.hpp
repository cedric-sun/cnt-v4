// cesun, 11/23/20 12:53 AM.

#ifndef CNT5106_V4_EVENTQUEUE_HPP
#define CNT5106_V4_EVENTQUEUE_HPP

#include <memory>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include "Event.hpp"

class EventQueue {
private:
    std::queue<std::unique_ptr<Event>> q;
    std::mutex m;
    std::condition_variable cond;
public:
    EventQueue() {}

    void enq(std::unique_ptr<Event> e) {
        {
            const std::lock_guard lg{m};
            q.push(std::move(e));
        }
        if (q.size() == 1)
            cond.notify_all();
    }

    std::unique_ptr<Event> deq() {
        std::unique_lock ul{m};
        cond.wait(ul, [&]() { return !q.empty(); });
        auto eup = std::move(q.front());
        q.pop();
        return eup;
    }
};


#endif //CNT5106_V4_EVENTQUEUE_HPP
