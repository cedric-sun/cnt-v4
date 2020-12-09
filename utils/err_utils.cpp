// cesun, 11/30/20 7:48 PM.

#include "err_utils.hpp"
#include <cerrno>
#include <execinfo.h>
#include <cstdio>
#include <unistd.h>

void panic(const std::string &msg) {
    if (errno != 0) {
        std::perror(msg.c_str());
    } else {
        fputs((msg + '\n').c_str(), ::stderr);
    }
    void *frame_ret[PANIC_BACKTRACE_FRAME_N];
    ::backtrace(frame_ret, PANIC_BACKTRACE_FRAME_N);
    ::backtrace_symbols_fd(frame_ret, PANIC_BACKTRACE_FRAME_N, STDERR_FILENO);
    ::exit(EXIT_FAILURE);
}

