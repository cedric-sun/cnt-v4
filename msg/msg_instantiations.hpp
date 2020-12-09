// cesun, 11/24/20 4:07 PM.

#ifndef CNT5106_V4_MSG_INSTANTIATIONS_HPP
#define CNT5106_V4_MSG_INSTANTIATIONS_HPP


#include "ZeroMsg.hpp"
#include "IndexMsg.hpp"

using ChokeMsg = ZeroMsg<MsgType::Choke>;
using UnchokeMsg = ZeroMsg<MsgType::Unchoke>;
using InterestedMsg = ZeroMsg<MsgType::Interested>;
using NotInterestedMsg = ZeroMsg<MsgType::NotInterested>;
using TearDownMsg = ZeroMsg<MsgType::TearDown>;
using HaveMsg = IndexMsg<MsgType::Have>;
using RequestMsg = IndexMsg<MsgType::Request>;


#endif //CNT5106_V4_MSG_INSTANTIATIONS_HPP
