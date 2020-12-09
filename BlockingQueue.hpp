// cesun, 12/9/20 6:04 PM.

#ifndef CNT5106_V4_BLOCKINGQUEUE_HPP
#define CNT5106_V4_BLOCKINGQUEUE_HPP

#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class BlockingQueue {
private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable cond;
public:
    BlockingQueue() = default;

    void enq(T e) {
        {
            const std::lock_guard lg{m};
            q.push(std::move(e));
        }
        if (q.size() == 1)
            cond.notify_all();
    }

    T deq() {
        std::unique_lock ul{m};
        cond.wait(ul, [&]() { return !q.empty(); });
        auto eup = std::move(q.front());
        q.pop();
        return eup;
    }

    [[nodiscard]] bool isEmpty() const {
        std::unique_lock ul{m};
        return q.empty();
    }
};


#endif //CNT5106_V4_BLOCKINGQUEUE_HPP
