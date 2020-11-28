// cesun, 11/28/20 2:36 AM.

#ifndef CNT5106_V4_EXISTINGFILE_HPP
#define CNT5106_V4_EXISTINGFILE_HPP

#include "File.hpp"
#include <filesystem>

class ExistingFile : public File {
private:
    const int64_t mSize;

    static FILE *openExisting(const std::string &path) {
        FILE *f = std::fopen(path.c_str(), "r");
        if (f == nullptr)
            panic("can't create new file for read only");
        return f;
    }

public:
    explicit ExistingFile(const std::string &path)
            : File{openExisting(path)},
              mSize{static_cast<int64_t>(std::filesystem::file_size(path))} {}

    void writeAt(int pos, const void *buf, int length) override {
        panic("file is not writable.");
    }

    [[nodiscard]] int64_t size() const override {
        return mSize;
    }
};


#endif //CNT5106_V4_EXISTINGFILE_HPP
