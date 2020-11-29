// cesun, 11/23/20 6:52 PM.

#ifndef CNT5106_V4_INDEXMSG_HPP
#define CNT5106_V4_INDEXMSG_HPP

#include "ActualMsg.hpp"

template<MsgType MT>
class IndexMsg : public ActualMsg {
    static_assert(MT == MsgType::Have || MT == MsgType::Request,
                  "Specified MsgType is not a IndexMsg");
protected:
    [[nodiscard]] int payloadSize() const override {
        return 4;
    }

    void writePayloadTo(BufferedWriter &w) const override {
        write32htonl(w, piece_id);
    }

public:
    const int piece_id;

    explicit IndexMsg(const int i) : ActualMsg{MT}, piece_id(i) {}

    IndexMsg(IndexMsg &&) noexcept = default;
};


#endif //CNT5106_V4_INDEXMSG_HPP
