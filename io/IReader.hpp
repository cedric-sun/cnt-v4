// cesun, 11/23/20 3:28 PM.

#ifndef CNT5106_V4_IREADER_HPP
#define CNT5106_V4_IREADER_HPP

#include "../cnt_types.hpp"
#include <cstdint>

class IReader {
public:
    // read at most `length` bytes into buf; return # of bytes actually read
    virtual int read(void *buf, int length) = 0;

    virtual ~IReader() = default;
};


#endif //CNT5106_V4_IREADER_HPP
