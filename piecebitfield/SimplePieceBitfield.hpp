// cesun, 11/23/20 6:59 PM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "../cnt_types.hpp"

// is 1 byte so that we don't need to worry about the byte order
enum class PieceStatus : Byte {
    ABSENT,
    REQUESTED,
    OWNED
};

#include <vector>
#include <mutex>
#include "../io/BufferedReader.hpp"
#include "../io/BufferedWriter.hpp"

class PieceBitfield {
protected:
    std::vector<PieceStatus> sv;

    PieceBitfield(std::vector<PieceStatus> sv) : sv{std::move(sv)} {}
public:
    // todo: ensure a will-be-moved-from object is never used by a second thread
    PieceBitfield(PieceBitfield &&other) : sv{std::move(other.sv)} {}

    void writeTo(BufferedWriter &w) const {
        // potentially sending REQUEST as well, but it doesn't matter
        w.write(sv.data(), sv.size());
    }

    int byteCount() const {
        return sv.size();
    }

    static PieceBitfield readFrom(BufferedReader &r, const int size) {
        std::vector<PieceStatus> tmp(size);
        r.read(tmp.data(), size);
        return PieceBitfield{std::move(tmp)};
    }
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
