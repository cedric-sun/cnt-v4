// cesun, 11/23/20 3:28 PM.

#ifndef CNT5106_V4_BUFFEREDREADER_HPP
#define CNT5106_V4_BUFFEREDREADER_HPP

#include "IReader.hpp"
#include "../cnt_types.hpp"
#include "../utils/class_utils.hpp"
#include <vector>
#include <cstring>
#include <cstdint>

class BufferedReader : public IReader {
private:
    IReader &r;
    std::vector<Byte> buffer = std::vector<Byte>(64 * 1024); // 64 KiB default; size = capacity
    int head{0}, tail{0};

    // precond: n <= buffer size
    void ensure(int n) {
        while (tail - head < n)
            flush();
    }

    void flush();

public:
    BufferedReader(IReader &r) : r{r} {}

    DISABLE_COPY_MOVE(BufferedReader)

    int read(void *buf, int length) override;

    // return the next byte
    Byte read() {
        ensure(1);
        return buffer[head++];
    }

    uint32_t read32() {
        ensure(4);
        auto ret = *reinterpret_cast<uint32_t *>(buffer.data() + head);
        head += 4;
        return ret;
    }
};


#endif //CNT5106_V4_BUFFEREDREADER_HPP
