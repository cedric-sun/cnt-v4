// cesun, 11/23/20 6:22 PM.

#include "HandshakeMsg.hpp"

const std::array<Byte, 28> HandshakeMsg::prefix = {
        'P', '2', 'P', 'F', 'I', 'L', 'E', 'S', 'H', 'A', 'R', 'I', 'N', 'G', 'P', 'R', 'O', 'J',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
