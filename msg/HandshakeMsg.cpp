// cesun, 11/23/20 6:22 PM.

#include "HandshakeMsg.hpp"
#include "../utils/err_utils.hpp"

const std::array<Byte, 28> HandshakeMsg::prefix = {
        'P', '2', 'P', 'F', 'I', 'L', 'E', 'S', 'H', 'A', 'R', 'I', 'N', 'G', 'P', 'R', 'O', 'J',
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

HandshakeMsg HandshakeMsg::readFrom(BufferedReader &r) {
    char buf[prefix.size()];
    r.read(buf, prefix.size());
    if (!std::equal(prefix.cbegin(), prefix.cend(), buf))
        panic("Mismatch in handshake prefix");
    return HandshakeMsg{read32ntohl(r)};
}
