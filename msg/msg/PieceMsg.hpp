// cesun, 11/23/20 5:39 PM.

#ifndef CNT5106_V4_PIECEMSG_HPP
#define CNT5106_V4_PIECEMSG_HPP

#include <memory>
#include "ActualMsg.hpp"
#include "../../storage/Piece.hpp"

class PieceMsg : public ActualMsg<MsgType::Piece> {
private:
    const int i;
    std::shared_ptr<Piece> piece;
public:
    PieceMsg(const int i, std::shared_ptr<Piece> piece) : i(i), piece{std::move(piece)} {}

protected:
    int payloadSize() const override {
        return 4 + piece->size();
    }

    void writePayloadTo(BufferedWriter &w) const override {
        write32htonl(w, i);
        w.write(piece->data(), piece->size());
    }
};


#endif //CNT5106_V4_PIECEMSG_HPP
