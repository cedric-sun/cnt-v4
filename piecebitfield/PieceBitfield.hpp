// cesun, 11/28/20 12:04 AM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "PieceStatus.hpp"
#include "../utils/class_utils.hpp"
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
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
