// cesun, 11/24/20 1:19 PM.

#ifndef CNT5106_V4_ASYNCMSGSCANNER_HPP
#define CNT5106_V4_ASYNCMSGSCANNER_HPP

#include "event/EventQueue.hpp"
#include "../io/BufferedReader.hpp"
#include "../utils/err_utils.hpp"
#include "../msg/HandshakeMsg.hpp"
#include "../msg/BitfieldMsg.hpp"
#include <thread>

class AsyncMsgScanner {
private:
    EventQueue &q;
    BufferedReader &br;
    std::optional<std::thread> tup;

    void scanLoop();

public:
    AsyncMsgScanner(BufferedReader &br, EventQueue &q) : q(q), br{br} {}

    void start() {
        tup.emplace(&AsyncMsgScanner::scanLoop, this);
    }

    void stop() {
        if (pthread_cancel(tup->native_handle()) != 0)
            panic("pthread_cancel failed");
        tup->join();
    }
};


#endif //CNT5106_V4_ASYNCMSGSCANNER_HPP
