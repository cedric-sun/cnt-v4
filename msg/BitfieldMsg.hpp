// cesun, 11/23/20 6:57 PM.

#ifndef CNT5106_V4_BITFIELDMSG_HPP
#define CNT5106_V4_BITFIELDMSG_HPP

#include "ActualMsg.hpp"
#include "../piecebitfield/SimplePieceBitfield.hpp"
#include "../utils/uptr_cast.hpp"
#include "../utils/err_utils.hpp"

class BitfieldMsg : public ActualMsg {
private:
    SimplePieceBitfield spbf;
protected:
    int payloadSize() const override {
        return spbf.byteCount();
    }

    void writePayloadTo(BufferedWriter &w) const override {
        //TODO: lock?
        spbf.writeTo(w);
    }

public:
    // force client to use SyncPieceBitfield::snapshot()
    BitfieldMsg(SimplePieceBitfield &&piecebf)
            : ActualMsg{MsgType::Bitfield}, spbf{std::move(piecebf)} {}

    SimplePieceBitfield extract() {
        return std::move(spbf);
    }

    static std::unique_ptr<BitfieldMsg> readFrom(BufferedReader &r) {
        auto msg = ActualMsg::readFrom(r);
        if (msg->type != MsgType::Bitfield)
            panic("Expecting Bitfield, but received something else.");
        return static_unique_ptr_cast<BitfieldMsg>(std::move(msg));
    }
};


#endif //CNT5106_V4_BITFIELDMSG_HPP
