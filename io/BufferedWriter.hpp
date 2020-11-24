// cesun, 11/23/20 5:41 PM.

#ifndef CNT5106_V4_BUFFEREDWRITER_HPP
#define CNT5106_V4_BUFFEREDWRITER_HPP

#include "IWriter.hpp"
#include "../cnt_types.hpp"
#include "../utils/class_utils.hpp"
#include <vector>
#include <cstdint>

class BufferedWriter : public IWriter {
private:
    IWriter &w;

    // [0,size()) is data, capacity doesn't change during lifetime.
    std::vector<Byte> buffer = std::vector<Byte>(64 * 1024);
public:
    BufferedWriter(IWriter &w) : w(w) {}

    DISABLE_COPY_MOVE(BufferedWriter)

    void write(const void *buf, int length) override {
        if (buffer.size() + length <= buffer.capacity()) {
            auto begin_cp = static_cast<const Byte *>(buf);
            buffer.insert(buffer.cend(), begin_cp, begin_cp + length);
        } else {
            //TODO: improve long write handling
            flush();
            w.write(buf, length);
        }
    }

    void write(Byte value) {
        buffer.push_back(value);
    }

    void write32(uint32_t value) {
        write(&value, 4);
    }

    void flush() {
        w.write(buffer.data(), buffer.size());
        buffer.clear();
    }
};


#endif //CNT5106_V4_BUFFEREDWRITER_HPP
