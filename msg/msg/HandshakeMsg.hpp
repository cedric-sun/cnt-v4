// cesun, 11/23/20 6:09 PM.

#ifndef CNT5106_V4_HANDSHAKEMSG_HPP
#define CNT5106_V4_HANDSHAKEMSG_HPP

#include "Msg.hpp"
#include <array>
#include "../../io/io_utils.hpp"

class HandshakeMsg : public Msg {
private:
    const int peer_id;
public:
    static const std::array<Byte, 28> prefix;

    HandshakeMsg(const int peer_id) : peer_id{peer_id} {}

    void writeTo(BufferedWriter &w) const override {
        w.write(prefix.data(), prefix.size());
        write32htonl(w, peer_id);
    }
};


#endif //CNT5106_V4_HANDSHAKEMSG_HPP
