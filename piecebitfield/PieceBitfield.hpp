// cesun, 11/26/20 1:01 AM.

#ifndef CNT5106_V4_PIECEBITFIELD_HPP
#define CNT5106_V4_PIECEBITFIELD_HPP

#include "../cnt_types.hpp"
#include <vector>

// is 1 byte so that we don't need to worry about the byte order
enum class PieceStatus : Byte {
    ABSENT,
    REQUESTED,
    OWNED
};

class PieceBitfield {
protected:
    std::vector<PieceStatus> sv;
public:
    explicit PieceBitfield(std::vector<PieceStatus> sv) : sv{std::move(sv)} {}
};


#endif //CNT5106_V4_PIECEBITFIELD_HPP
