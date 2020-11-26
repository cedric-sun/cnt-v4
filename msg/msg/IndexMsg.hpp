// cesun, 11/23/20 6:52 PM.

#ifndef CNT5106_V4_INDEXMSG_HPP
#define CNT5106_V4_INDEXMSG_HPP

#include "ActualMsg.hpp"

template<MsgType MT>
class IndexMsg : public ActualMsg {
    static_assert(MT == MsgType::Have || MT == MsgType::Request,
                  "Specified MsgType is not a IndexMsg");
private:
    const int i;
protected:
    int payloadSize() const override {
        return 4;
    }

    void writePayloadTo(BufferedWriter &w) const override {
        write32htonl(w, i);
    }

public:
    explicit IndexMsg(const int i) : ActualMsg{MT}, i(i) {}

    IndexMsg(IndexMsg &&) noexcept = default;
};


#endif //CNT5106_V4_INDEXMSG_HPP
