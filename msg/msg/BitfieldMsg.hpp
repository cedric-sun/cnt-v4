// cesun, 11/23/20 6:57 PM.

#ifndef CNT5106_V4_BITFIELDMSG_HPP
#define CNT5106_V4_BITFIELDMSG_HPP

#include "ActualMsg.hpp"
#include "../../PieceBitfield.hpp"

class BitfieldMsg : public ActualMsg {
private:
    PieceBitfield piecebf;
protected:
    int payloadSize() const override {
        return piecebf.byteCount();
    }

    void writePayloadTo(BufferedWriter &w) const override {
        //TODO: lock?
        piecebf.writeTo(w);
    }

public:
    BitfieldMsg(PieceBitfield piecebf)
            : ActualMsg{MsgType::Bitfield}, piecebf{std::move(piecebf)} {}
};


#endif //CNT5106_V4_BITFIELDMSG_HPP
