// cesun, 11/23/20 5:39 PM.

#ifndef CNT5106_V4_MSGFACTORY_HPP
#define CNT5106_V4_MSGFACTORY_HPP

#include "msg/HandshakeMsg.hpp"
#include "../storage/PieceRepository.hpp"

#include "msg/ZeroMsg.hpp"
#include "msg/IndexMsg.hpp"
#include "msg/BitfieldMsg.hpp"
#include "msg/PieceMsg.hpp"

#include <unordered_map>

using ChokeMsg = ZeroMsg<MsgType::Choke>;
using UnchokeMsg = ZeroMsg<MsgType::Unchoke>;
using InterestedMsg = ZeroMsg<MsgType::Interested>;
using NotInterestedMsg = ZeroMsg<MsgType::NotInterested>;
using HaveMsg = IndexMsg<MsgType::Have>;
using RequestMsg = IndexMsg<MsgType::Request>;

class MsgFactory {
private:
    const int self_peer_id;
    PieceRepository &pr;

    template<typename M>
    [[nodiscard]] const M &getZeroMsg() const {
        //TODO meta ensure M is a ZeroMsg
        static M instance;
        return instance;
    }

    template<typename M>
    [[nodiscard]] const M &getIndexMsg(const int i) const {
        //TODO meta ensure M is a IndexMsg
        static std::unordered_map<int, M> cache;
        auto it = cache.find(i);
        if (it == cache.cend()) {
            // IndexMsg is not movable but is trivially copyable
            it = cache.emplace(i, M{i}).first;
        }
        return it->second;
    }

public:
    MsgFactory(const int self_peer_id, PieceRepository &pr)
            : self_peer_id{self_peer_id}, pr{pr} {
    }

    [[nodiscard]] const HandshakeMsg &getHandshakeMsg() const {
        static HandshakeMsg hs{self_peer_id}; // all compilation unit see unique instance
        return hs;
    }

    // these functions instantiate function templates, and is non-trivial to be inline
    // TODO: move them into cpp file
    [[nodiscard]] decltype(auto) getChokeMsg() const { return getZeroMsg<ChokeMsg>(); }

    [[nodiscard]] decltype(auto) getUnchokeMsg() const { return getZeroMsg<UnchokeMsg>(); }

    [[nodiscard]] decltype(auto) getInterestedMsg() const { return getZeroMsg<InterestedMsg>(); }

    [[nodiscard]] decltype(auto) getNotInterestedMsg() const { return getZeroMsg<NotInterestedMsg>(); }

    [[nodiscard]] decltype(auto) getHaveMsg(const int i) const { return getIndexMsg<HaveMsg>(i); }

    [[nodiscard]] const BitfieldMsg &getBitfieldMsg(const int i) const {
        //TODO
    }

    [[nodiscard]] decltype(auto) getRequestMsg(const int i) const {
        return getIndexMsg<RequestMsg>(i);
    }

    [[nodiscard]] const PieceMsg &getPieceMsg(const int i) const {
        //TODO
    }
};


#endif //CNT5106_V4_MSGFACTORY_HPP
