// cesun, 11/28/20 2:35 AM.

#ifndef CNT5106_V4_NEWFILE_HPP
#define CNT5106_V4_NEWFILE_HPP

#include "File.hpp"
#include <mutex>

class NewFile : public File {
private:
    const int64_t mSize;
    mutable std::mutex fseek_fwrite_atomicity_mutex;

    static FILE *openNew(const std::string &path) {
        FILE *f = std::fopen(path.c_str(), "w+");
        if (f == nullptr)
            panic("can't create new file for read & write");
        return f;
    }

public:
    explicit NewFile(const std::string &path, const int64_t size)
            : File{openNew(path)}, mSize{size} {}

    void writeAt(int64_t pos, const void *buf, int64_t length) override {
        const std::lock_guard lg{fseek_fwrite_atomicity_mutex};
        if (std::fseek(f, pos, SEEK_SET) != 0)
            panic("fseek failed");
        if (std::fwrite(buf, 1, length, f) != length)
            panic("less-than-expected number of bytes is written by fwrite()");
    }

    [[nodiscard]] int64_t size() const override {
        return mSize;
    }
};


#endif //CNT5106_V4_NEWFILE_HPP
