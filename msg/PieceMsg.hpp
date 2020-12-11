// cesun, 11/23/20 5:39 PM.

#ifndef CNT5106_V4_PIECEMSG_HPP
#define CNT5106_V4_PIECEMSG_HPP

#include <memory>
#include "ActualMsg.hpp"
#include "../storage/Piece.hpp"
#include "../utils/class_utils.hpp"
#include <openssl/md5.h>

class PieceMsg : public ActualMsg {
private:
    std::shared_ptr<Piece> piece;
public:
    const int piece_id;

    PieceMsg(const int i, std::shared_ptr<Piece> piece)
            : ActualMsg{MsgType::Piece}, piece_id(i), piece{std::move(piece)} {}

    DFT_MOVE_CTOR_ONLY(PieceMsg)

    // return a moved piece shared_ptr; UB when called twice;
    std::shared_ptr<Piece> getPiece() {
        return std::move(piece);
    }

protected:
    [[nodiscard]] int payloadSize() const override {
        return 4 + MD5_DIGEST_LENGTH + piece->size();
    }

    void writePayloadTo(BufferedWriter &w) const override {
        write32htonl(w, piece_id);
        // TODO: cache md5 result instead of computing it every time
        std::array<Byte, MD5_DIGEST_LENGTH> md5_buf;
        ::MD5(piece->data(), piece->size(), md5_buf.data());
        w.write(md5_buf.data(), md5_buf.size());
        w.write(piece->data(), piece->size());
    }
};


#endif //CNT5106_V4_PIECEMSG_HPP
