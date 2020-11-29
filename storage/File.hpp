// cesun, 11/23/20 5:42 PM.

#ifndef CNT5106_V4_FILE_HPP
#define CNT5106_V4_FILE_HPP

#include <cstdio>
#include "../utils/class_utils.hpp"
#include "../utils/err_utils.hpp"

class File {
protected:
    std::FILE *f;

    explicit File(std::FILE *f) : f{f} {}

public:

    DISABLE_COPY_MOVE(File)

    virtual ~File() { std::fclose(f); }

    void readAt(int64_t pos, void *buf, int64_t length) {
        if (std::fseek(f, pos, SEEK_SET) != 0)
            panic("fseek failed");
        if (std::fread(buf, 1, length, f) != length)
            panic("less-than-expected number of bytes is read by fread()");
    }

    virtual void writeAt(int64_t pos, const void *buf, int64_t length) = 0;

    [[nodiscard]] virtual int64_t size() const = 0;
};


#endif //CNT5106_V4_FILE_HPP
