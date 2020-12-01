// cesun, 11/28/20 11:35 PM.

#ifndef CNT5106_V4_PIECEBITFIELDSNAPSHOT_HPP
#define CNT5106_V4_PIECEBITFIELDSNAPSHOT_HPP

#include "../io/BufferedWriter.hpp"
#include "../io/BufferedReader.hpp"
#include "PieceBitfield.hpp"

class PieceBitfieldSnapshot {
    friend PieceBitfield::PieceBitfield(PieceBitfieldSnapshot &&);

private:
    std::vector<PieceStatus> sv;
public:
    explicit PieceBitfieldSnapshot(std::vector<PieceStatus> sv) : sv{std::move(sv)} {}

    DFT_MOVE_CTOR_ONLY(PieceBitfieldSnapshot)

    [[nodiscard]] int byteCount() const {
        return sv.size();
    }

    void writeTo(BufferedWriter &w) const {
        w.write(sv.data(), sv.size());
    }

    static PieceBitfieldSnapshot readFrom(BufferedReader &r, const int size) {
        std::vector<PieceStatus> tmp(size);
        r.read(tmp.data(), size);
        return PieceBitfieldSnapshot{std::move(tmp)};
    }
};


#endif //CNT5106_V4_PIECEBITFIELDSNAPSHOT_HPP
