// cesun, 11/23/20 4:03 PM.

#ifndef CNT5106_V4_ERR_UTILS_HPP
#define CNT5106_V4_ERR_UTILS_HPP

#include <cerrno>
#include <execinfo.h>
#include <cstdio>
#include <unistd.h>
#include <string>

[[noreturn]] void panic(const std::string &msg) {
    if (errno != 0)
        std::perror(msg.c_str());
    else
        fputs(msg.c_str(), ::stderr);
    void *frame_ret[PANIC_BACKTRACE_FRAME_N];
    ::backtrace(frame_ret, PANIC_BACKTRACE_FRAME_N);
    ::backtrace_symbols_fd(frame_ret, PANIC_BACKTRACE_FRAME_N, STDERR_FILENO);
    ::exit(EXIT_FAILURE);
}

#endif //CNT5106_V4_ERR_UTILS_HPP
