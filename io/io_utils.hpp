// cesun, 11/23/20 3:52 PM.

#ifndef CNT5106_V4_IO_UTILS_HPP
#define CNT5106_V4_IO_UTILS_HPP

#include "BufferedReader.hpp"
#include "BufferedWriter.hpp"
#include <arpa/inet.h>

inline int read32ntohl(BufferedReader &r) {
    return static_cast<int>(ntohl(r.read32()));
}

inline void write32htonl(BufferedWriter &w, int value) {
    w.write32(htonl(static_cast<uint32_t>(value)));
}


#endif //CNT5106_V4_IO_UTILS_HPP
