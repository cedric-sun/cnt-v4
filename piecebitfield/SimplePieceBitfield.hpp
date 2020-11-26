// cesun, 11/23/20 6:59 PM.

#ifndef CNT5106_V4_SIMPLEPIECEBITFIELD_HPP
#define CNT5106_V4_SIMPLEPIECEBITFIELD_HPP

#include "PieceStatus.hpp"
#include "../io/BufferedReader.hpp"
#include "../io/BufferedWriter.hpp"
#include "../utils/class_utils.hpp"

// SimplePieceBitfield meant to be used in single thread
class SimplePieceBitfield {
private:
    std::vector<PieceStatus> sv;
public:
    explicit SimplePieceBitfield() = delete;

    explicit SimplePieceBitfield(std::vector<PieceStatus> sv)
            : sv{std::move(sv)} {}

    DISABLE_COPY(SimplePieceBitfield)

    SimplePieceBitfield(SimplePieceBitfield &&) = default;

    SimplePieceBitfield &operator=(SimplePieceBitfield &&) = delete;

    void writeTo(BufferedWriter &w) const {
        // potentially sending REQUEST as well, but it doesn't matter
        w.write(sv.data(), sv.size());
    }

    int byteCount() const {
        return sv.size();
    }

    static SimplePieceBitfield readFrom(BufferedReader &r, const int size) {
        std::vector<PieceStatus> tmp(size);
        r.read(tmp.data(), size);
        return SimplePieceBitfield{std::move(tmp)};
    }
};


#endif //CNT5106_V4_SIMPLEPIECEBITFIELD_HPP
