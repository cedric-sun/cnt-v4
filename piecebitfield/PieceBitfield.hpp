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
private:
    std::vector<PieceStatus> sv;
    int n_owned;

protected:
    explicit PieceBitfield(const int size, bool owningAllPiece)
            : sv(size, owningAllPiece ? PieceStatus::OWNED : PieceStatus::ABSENT),
              n_owned{owningAllPiece ? size : 0} {}

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

    // returns a vector of sorted indexes the piece-bit at which is OWNED in lhs but ABSENT in rhs
    std::vector<int> operator-(const SyncPieceBitfield &rhs) const;

    virtual void setOwned(const int i) {
        if (i < 0 || i >= sv.size())
            panic("setOwn index out of bound.");
        if (sv[i] == PieceStatus::OWNED)
            panic("piece already owned!");
        sv[i] = PieceStatus::OWNED;
        n_owned++;
    }

    [[nodiscard]]  virtual bool owningAll() const {
        return n_owned == sv.size();
        // TODO: improve concurrent performance
        //      In the thread-safe derived class, we acquires its lock purely for testing this
        //      condition here;
    }
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
