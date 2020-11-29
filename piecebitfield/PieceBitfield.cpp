// cesun, 11/28/20 12:37 AM.

#include "PieceBitfield.hpp"
#include "SyncPieceBitfield.hpp"

std::vector<int> PieceBitfield::operator-(const SyncPieceBitfield &rhs) const {
    std::lock_guard lg{rhs};
    if (sv.size() != rhs.sv.size())
        panic("operands size is not identical");
    std::vector<int> ret;
    for (int i = 0; i < sv.size(); ++i) {
        if (sv[i] == PieceStatus::OWNED && rhs.sv[i] == PieceStatus::ABSENT)
            ret.push_back(i);
    }
    return ret;
}
