// cesun, 11/23/20 3:28 PM.

#include "BufferedReader.hpp"

void BufferedReader::flush() {
    if (head != 0) {
        const int len = tail - head;
        std::memmove(buffer.data(), buffer.data() + head, len);
        head = 0;
        tail = len;
    }
    tail += r.read(buffer.data() + tail, buffer.size() - tail);
}

int BufferedReader::read(void *buf, int length) {
    if (length > buffer.size()) { // long read TODO: check correctness
        auto *begin = static_cast<Byte *>(buf);
        auto *end = begin + read(buf, tail - head); // consume everything in current buffer
        while (end - begin < length) {
            end += r.read(end, begin + length - end);
        }
    } else {
        ensure(length);
        std::memcpy(buf, buffer.data() + head, length);
        head += length;
    }
    return length;
}
