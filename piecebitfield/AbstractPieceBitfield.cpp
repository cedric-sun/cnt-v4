// cesun, 12/1/20 1:01 PM.

#include "AbstractPieceBitfield.hpp"
#include <mutex>

std::vector<int> AbstractPieceBitfield::operator-(const AbstractPieceBitfield &rhs) const {
    if (sv.size() != rhs.sv.size())
        panic("operands size is not identical");
    std::scoped_lock sl{*this, rhs};
    std::vector<int> ret;
    for (int i = 0; i < sv.size(); ++i) {
        if (sv[i] == PieceStatus::OWNED && rhs.sv[i] == PieceStatus::ABSENT)
            ret.push_back(i);
    }
    return ret;
}
