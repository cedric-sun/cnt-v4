// cesun, 11/28/20 12:04 AM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "PieceStatus.hpp"
#include "../utils/class_utils.hpp"
#include "../io/BufferedWriter.hpp"
#include "../io/BufferedReader.hpp"
#include <vector>

class PieceBitfield {
protected:
    std::vector<PieceStatus> sv;
public:
    PieceBitfield() = delete;

    explicit PieceBitfield(std::vector<PieceStatus> sv) : sv{std::move(sv)} {}

    DISABLE_COPY(PieceBitfield)

    DEFAULT_MOVE(PieceBitfield)

    virtual ~PieceBitfield() = default;

    // returns a vector of sorted indexes the piece-bit at which is OWNED in lhs but ABSENT in rhs
    std::vector<int> operator-(const PieceBitfield &rhs) const;

    PieceStatus& operator[](const int i) {
        return sv[i];
    }

    void writeTo(BufferedWriter &w) const {
        // potentially sending REQUEST as well, but it doesn't matter
        w.write(sv.data(), sv.size());
    }

    [[nodiscard]] int byteCount() const {
        return sv.size();
    }

    static PieceBitfield readFrom(BufferedReader &r, const int size) {
        std::vector<PieceStatus> tmp(size);
        r.read(tmp.data(), size);
        return PieceBitfield{std::move(tmp)};
    }
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
