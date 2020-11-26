// cesun, 11/26/20 1:01 AM.

#ifndef CNT5106_V4_PIECESTATUS_HPP
#define CNT5106_V4_PIECESTATUS_HPP

#include "../cnt_types.hpp"

// is 1 byte so that we don't need to worry about the byte order
enum class PieceStatus : Byte {
    ABSENT,
    REQUESTED,
    OWNED
};


#endif //CNT5106_V4_PIECESTATUS_HPP
