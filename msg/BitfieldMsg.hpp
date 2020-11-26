// cesun, 11/23/20 6:57 PM.

#ifndef CNT5106_V4_BITFIELDMSG_HPP
#define CNT5106_V4_BITFIELDMSG_HPP

#include "ActualMsg.hpp"
#include "../../PieceBitfield.hpp"
#include "../../utils/uptr_cast.hpp"
#include "../../utils/err_utils.hpp"

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

    PieceBitfield extract() {
        return std::move(piecebf);
    }

    static std::unique_ptr<BitfieldMsg> readFrom(BufferedReader &r) {
        auto msg = ActualMsg::readFrom(r);
        if (msg->type != MsgType::Bitfield)
            panic("Expecting Bitfield, but received something else.");
        return static_unique_ptr_cast<BitfieldMsg>(std::move(msg));
    }
};


#endif //CNT5106_V4_BITFIELDMSG_HPP
