// cesun, 11/23/20 6:08 PM.

#ifndef CNT5106_V4_ACTUALMSG_HPP
#define CNT5106_V4_ACTUALMSG_HPP

#include "Msg.hpp"
#include "../../io/io_utils.hpp"
#include "MsgType.hpp"
#include <memory>

class ActualMsg : public Msg {
public:
    const MsgType type;
protected:
    virtual int payloadSize() const = 0;

    virtual void writePayloadTo(BufferedWriter &w) const = 0;

public:
    explicit ActualMsg(const MsgType type) : type(type) {}

    static std::unique_ptr<ActualMsg> readFrom(BufferedReader &r);

    void writeTo(BufferedWriter &w) const final {
        write32htonl(w, payloadSize() + 1); // +1 for MsgType
        w.write(static_cast<Byte>(type));
        writePayloadTo(w);
    }
};


#endif //CNT5106_V4_ACTUALMSG_HPP
