// cesun, 12/4/20 5:01 PM.

#include "PieceBitfield.hpp"
#include "PieceBitfieldSnapshot.hpp"

PieceBitfield::PieceBitfield(PieceBitfieldSnapshot &&pb_snap)
        : AbstractPieceBitfield{std::move(pb_snap.sv)} {
    n_owned = 0;
    for (const auto &e : sv) {
        if (e == PieceStatus::OWNED)
            n_owned++;
    }
}
