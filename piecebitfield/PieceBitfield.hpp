// cesun, 11/28/20 12:04 AM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "AbstractPieceBitfield.hpp"
#include "../utils/class_utils.hpp"
#include "../utils/err_utils.hpp"

class PieceBitfieldSnapshot;

class PieceBitfield : public AbstractPieceBitfield {
private:
    int n_owned;
public:
    void lock() const override {}

    void unlock() const override {}

    bool try_lock() const override { return true; } // todo ensure `true` is ok for noop try_lock

public:
    explicit PieceBitfield(PieceBitfieldSnapshot &&pb_snap);

    DFT_MOVE_CTOR_ONLY(PieceBitfield)

    virtual ~PieceBitfield() = default;


    bool isOwned(const int i) const override {
        checkRange(i);
        return sv[i] == PieceStatus::OWNED;
    }

    void setOwned(const int i) override {
        checkRange(i);
        if (sv[i] != PieceStatus::OWNED)
            n_owned++;
        sv[i] = PieceStatus::OWNED;
    }

    bool owningAll() const override {
        return n_owned == sv.size();
    }
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
