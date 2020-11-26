// cesun, 11/23/20 5:39 PM.

#ifndef CNT5106_V4_MSGFACTORY_HPP
#define CNT5106_V4_MSGFACTORY_HPP

#include "../storage/PieceRepository.hpp"
#include "msg/HandshakeMsg.hpp"
#include "msg/BitfieldMsg.hpp"
#include "msg/PieceMsg.hpp"
#include "msg/msg_instantiations.hpp"

#include <unordered_map>
#include <shared_mutex>

class MsgFactory {
private:
    const int self_peer_id;
    PieceRepository &pr;

    template<typename M>
    [[nodiscard]] const M &getZeroMsg() const {
        //TODO meta ensure M is a ZeroMsg instantiation
        static M instance;
        return instance;
    }

    template<typename M>
    [[nodiscard]] const M &getIndexMsg(const int i) const {
        //TODO meta ensure M is a IndexMsg instantiation
        static std::shared_mutex sm;
        static std::unordered_map<int, M> cache;
        {
            const std::shared_lock sl{sm};
            if (auto it = cache.find(i);it != cache.cend()) {
                return it->second;
            }
        }
        const std::lock_guard lg{sm};
        auto it = cache.emplace(i, M{i}).first;
        return it->second;
    }

public:
    MsgFactory(const int self_peer_id, PieceRepository &pr)
            : self_peer_id{self_peer_id}, pr{pr} {
    }

    DISABLE_COPY_MOVE(MsgFactory)

    [[nodiscard]] const HandshakeMsg &getHandshakeMsg() const {
        static HandshakeMsg hs{self_peer_id}; // all compilation unit see unique instance
        return hs;
    }

    [[nodiscard]] decltype(auto) getChokeMsg() const { return getZeroMsg<ChokeMsg>(); }

    [[nodiscard]] decltype(auto) getUnchokeMsg() const { return getZeroMsg<UnchokeMsg>(); }

    [[nodiscard]] decltype(auto) getInterestedMsg() const { return getZeroMsg<InterestedMsg>(); }

    [[nodiscard]] decltype(auto)
    getNotInterestedMsg() const { return getZeroMsg<NotInterestedMsg>(); }

    [[nodiscard]] decltype(auto) getHaveMsg(const int i) const { return getIndexMsg<HaveMsg>(i); }

    [[nodiscard]] const BitfieldMsg &getBitfieldMsg() const {
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
