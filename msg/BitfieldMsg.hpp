// cesun, 11/23/20 6:57 PM.

#ifndef CNT5106_V4_BITFIELDMSG_HPP
#define CNT5106_V4_BITFIELDMSG_HPP

#include "ActualMsg.hpp"
#include "../piecebitfield/PieceBitfieldSnapshot.hpp"
#include "../utils/err_utils.hpp"

class BitfieldMsg : public ActualMsg {
private:
    PieceBitfieldSnapshot snapshot;
protected:
    [[nodiscard]] int payloadSize() const override {
        return snapshot.byteCount();
    }

    void writePayloadTo(BufferedWriter &w) const override {
        //TODO: lock?
        snapshot.writeTo(w);
    }

public:
    explicit BitfieldMsg(PieceBitfieldSnapshot &&snapshot)
            : ActualMsg{MsgType::Bitfield}, snapshot{std::move(snapshot)} {}

    PieceBitfieldSnapshot extract() {
        return std::move(snapshot);
    }

    static std::unique_ptr<BitfieldMsg> readFrom(BufferedReader &r) {
        auto msg_up = ActualMsg::readFrom(r);
        if (msg_up->type != MsgType::Bitfield)
            panic("Expecting Bitfield, but received something else.");
        return std::unique_ptr<BitfieldMsg>{static_cast<BitfieldMsg*>(msg_up.release())};
    }
};


#endif //CNT5106_V4_BITFIELDMSG_HPP
