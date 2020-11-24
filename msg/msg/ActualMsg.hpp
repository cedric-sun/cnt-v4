// cesun, 11/23/20 6:08 PM.

#ifndef CNT5106_V4_ACTUALMSG_HPP
#define CNT5106_V4_ACTUALMSG_HPP

#include "Msg.hpp"
#include "../../io/io_utils.hpp"
#include "MsgType.hpp"

template<MsgType MT>
class ActualMsg : public Msg {
protected:
    virtual int payloadSize() const = 0;

    virtual void writePayloadTo(BufferedWriter &w) const = 0;

public:
    void writeTo(BufferedWriter &w) const final {
        write32htonl(w, payloadSize() + 1); // +1 for MsgType
        w.write(static_cast<Byte>(MT));
        writePayloadTo(w);
    }
};


#endif //CNT5106_V4_ACTUALMSG_HPP
