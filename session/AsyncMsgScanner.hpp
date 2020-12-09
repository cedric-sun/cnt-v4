// cesun, 11/24/20 1:19 PM.

#ifndef CNT5106_V4_ASYNCMSGSCANNER_HPP
#define CNT5106_V4_ASYNCMSGSCANNER_HPP

#include "../utils/err_utils.hpp"
#include "../utils/class_utils.hpp"
#include "event/EventQueue.hpp"
#include <thread>

class BufferedReader;

class AsyncMsgScanner {
public:
    DISABLE_COPY_MOVE(AsyncMsgScanner)
private:
    EventQueue &q;
    BufferedReader &br;
    std::optional<std::jthread> tup{std::nullopt};

    void scanLoop();

public:
    AsyncMsgScanner(BufferedReader &br, EventQueue &q) : q(q), br{br} {}

    void start() {
        tup.emplace(&AsyncMsgScanner::scanLoop, this);
    }

    void stop() {
        if (pthread_cancel(tup->native_handle()) != 0)
            panic("pthread_cancel failed");
    }
};


#endif //CNT5106_V4_ASYNCMSGSCANNER_HPP
