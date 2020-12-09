// cesun, 11/23/20 6:37 PM.

#ifndef CNT5106_V4_ZEROMSG_HPP
#define CNT5106_V4_ZEROMSG_HPP

#include "ActualMsg.hpp"

template<MsgType MT>
class ZeroMsg : public ActualMsg {
    static_assert(MT == MsgType::Choke || MT == MsgType::Unchoke
                  || MT == MsgType::Interested || MT == MsgType::NotInterested ||
                  MT == MsgType::TearDown,
                  "Specified MsgType is not a ZeroMsg");
protected:
    [[nodiscard]] int payloadSize() const override { return 0; }

    void writePayloadTo(BufferedWriter &w) const override {
        //noop
    }

public:
    explicit ZeroMsg() : ActualMsg{MT} {}
};


#endif //CNT5106_V4_ZEROMSG_HPP
