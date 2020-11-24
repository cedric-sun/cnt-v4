// cesun, 11/23/20 6:57 PM.

#ifndef CNT5106_V4_BITFIELDMSG_HPP
#define CNT5106_V4_BITFIELDMSG_HPP

#include "ActualMsg.hpp"
#include "../../PieceBitfield.hpp"

class BitfieldMsg : public ActualMsg<MsgType::Bitfield> {
private:
    PieceBitfield pb;
protected:
    int payloadSize() const override {
        return 0;
    }

    void writePayloadTo(BufferedWriter &w) const override {
        //TODO
    }
};


#endif //CNT5106_V4_BITFIELDMSG_HPP
