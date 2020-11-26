// cesun, 11/24/20 1:19 PM.

#ifndef CNT5106_V4_ASYNCMSGSCANNER_HPP
#define CNT5106_V4_ASYNCMSGSCANNER_HPP

#include "event/EventQueue.hpp"
#include "../io/BufferedReader.hpp"
#include "../utils/err_utils.hpp"
#include "../msg/msg/HandshakeMsg.hpp"
#include "../msg/msg/BitfieldMsg.hpp"
#include <thread>

class AsyncMsgScanner {
private:
    EventQueue &q;
    BufferedReader br;
    std::unique_ptr<std::thread> tup{nullptr};

    void scanLoop();

public:
    AsyncMsgScanner(IReader &r, EventQueue &q) : q(q), br{r} {}

    void start() {
        tup = std::make_unique<std::thread>(&AsyncMsgScanner::scanLoop, this);
    }

    void stop() {
        if (pthread_cancel(tup->native_handle()) != 0)
            panic("pthread_cancel failed");
        tup->join();
    }
};


#endif //CNT5106_V4_ASYNCMSGSCANNER_HPP
