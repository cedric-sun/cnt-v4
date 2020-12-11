// cesun, 11/24/20 2:47 PM.

#include "ActualMsg.hpp"
#include "msg_instantiations.hpp"
#include "BitfieldMsg.hpp"
#include "PieceMsg.hpp"

std::unique_ptr<ActualMsg> ActualMsg::readFrom(BufferedReader &r) {
    std::unique_ptr<ActualMsg> ret{nullptr};
    const int n = read32ntohl(r);
    auto type = static_cast<MsgType>(r.read());
    //TODO: use meta magic to map enum to type
    switch (type) {
        case MsgType::Choke:
            ret = std::make_unique<ChokeMsg>();
            break;
        case MsgType::Unchoke:
            ret = std::make_unique<UnchokeMsg>();
            break;
        case MsgType::Interested:
            ret = std::make_unique<InterestedMsg>();
            break;
        case MsgType::NotInterested:
            ret = std::make_unique<NotInterestedMsg>();
            break;
        case MsgType::TearDown:
            ret = std::make_unique<TearDownMsg>();
            break;
        case MsgType::Have:
            ret = std::make_unique<HaveMsg>(read32ntohl(r));
            break;
        case MsgType::Bitfield: {
            auto snapshot = PieceBitfieldSnapshot::readFrom(r, n - 1);
            ret = std::make_unique<BitfieldMsg>(std::move(snapshot));
        }
            break;
        case MsgType::Request:
            ret = std::make_unique<RequestMsg>(read32ntohl(r));
            break;
        case MsgType::Piece: {
            auto piece_id = read32ntohl(r);
            std::array<Byte, MD5_DIGEST_LENGTH> peer_md5;
            std::array<Byte, MD5_DIGEST_LENGTH> self_md5;
            r.read(peer_md5.data(), peer_md5.size());
            auto piece_sp = std::make_shared<Piece>(n - 1 - 4 - MD5_DIGEST_LENGTH);
            r.read(piece_sp->data(), piece_sp->size());
            ::MD5(piece_sp->data(), piece_sp->size(), self_md5.data());
            if (peer_md5 != self_md5)
                panic("MD5 mismatch for piece id = " + std::to_string(piece_id));
            ret = std::make_unique<PieceMsg>(piece_id, std::move(piece_sp));
        }
            break;
        default:
            panic("Unknown message type.");
    }
    return ret;
}

