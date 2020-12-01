// cesun, 11/28/20 12:04 AM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "PieceStatus.hpp"
#include "PieceBitfieldSnapshot.hpp"
#include "../utils/class_utils.hpp"
#include "../utils/err_utils.hpp"
#include <vector>

class SyncPieceBitfield;

class PieceBitfield {
protected:
    int n_owned;
    std::vector<PieceStatus> sv;

    explicit PieceBitfield(const int size, bool owningAllPiece)
            : sv(size, owningAllPiece ? PieceStatus::OWNED : PieceStatus::ABSENT),
              n_owned{owningAllPiece ? size : 0} {}

    void assertRange(const int i) {
        if (i < 0 || i >= sv.size())
            panic("setOwn index out of bound.");
    }

    void setOwnedUnsafe(const int i) {
        sv[i] = PieceStatus::OWNED;
        n_owned++;
    }

public:
    explicit PieceBitfield(PieceBitfieldSnapshot &&pb_snap) : sv{std::move(pb_snap.sv)} {
        n_owned = 0;
        for (const auto &e : sv) {
            if (e == PieceStatus::OWNED)
                n_owned++;
        }
    }

    DFT_MOVE_CTOR_ONLY(PieceBitfield)

    virtual ~PieceBitfield() = default;

    // thread safe for rhs
    // returns a vector of sorted indexes the piece-bit at which is OWNED in lhs but ABSENT in rhs
    std::vector<int> operator-(const SyncPieceBitfield &rhs) const;

    virtual bool isOwned(const int i) {
        assertRange(i);
        return sv[i] == PieceStatus::OWNED;
    }

    virtual void setOwned(const int i) {
        assertRange(i);
        setOwnedUnsafe(i);
    }

    [[nodiscard]]  virtual bool owningAll() const {
        return n_owned == sv.size();
    }
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
