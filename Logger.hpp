// cesun, 11/28/20 3:38 AM.

#ifndef CNT5106_V4_LOGGER_HPP
#define CNT5106_V4_LOGGER_HPP

#include <cstdio>
#include <cstdarg>
#include <string>
#include <vector>
#include "utils/err_utils.hpp"

class Logger {
private:
    const int self_peer_id;
    std::FILE *log_stream;

    void realLogPrintf(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        std::vfprintf(log_stream, fmt, args);
        va_end(args);
    }

    static std::string nowTime();

    static std::string strVector(const std::vector<int> &vec);

public:
    Logger(const int self_peer_id, const std::string &path)
            : self_peer_id{self_peer_id} {
        log_stream = std::fopen(path.c_str(), "w");
        ::setvbuf(log_stream, nullptr, _IOLBF, 0);
        if (log_stream == nullptr)
            panic("can't open log file for write");
    }

    ~Logger() {
        std::fclose(log_stream);
    }

#define logPrintf(fmt, ...) realLogPrintf("[%s] " fmt, nowTime().c_str(), __VA_ARGS__)

    void selfConnectedTo(int server_peer_id) {
        logPrintf("Peer [%d] makes a connection to Peer [%d].\n", self_peer_id, server_peer_id);
    }

    void selfConnectedBy(int client_peer_id) {
        logPrintf("Peer [%d] is connected from Peer [%d].\n", self_peer_id, client_peer_id);
    }

    void newPreferredNeighbors(const std::vector<int> &peer_ids) {
        logPrintf("Peer [%d] has the preferred neighbors [%s].\n", self_peer_id,
                  strVector(peer_ids).c_str());
    }

    void newOptUnchokedNeighbor(int peer_id) {
        logPrintf("Peer [%d] has the optimistically unchoked neighbor [%d].\n", self_peer_id,
                  peer_id);
    }

    void unchokeReceived(int peer_id) {
        logPrintf("Peer [%d] is unchoked by [%d].\n", self_peer_id, peer_id);
    }

    void chokeReceived(int peer_id) {
        logPrintf("Peer [%d] is choked by [%d].\n", self_peer_id, peer_id);
    }

    void haveReceived(int peer_id, int piece_idx) {
        logPrintf("Peer [%d] received the ‘have’ message from [%d] for the piece [%d].\n",
                  self_peer_id, peer_id, piece_idx);
    }

    void interestedReceived(int peer_id) {
        logPrintf("Peer [%d] received the ‘interested’ message from [%d].\n", self_peer_id,
                  peer_id);
    }

    void NotInterestedReceived(int peer_id) {
        logPrintf("Peer [%d] received the ‘not interested’ message from [%d].\n", self_peer_id,
                  peer_id);
    }

    void pieceDownloaded(int peer_id, int piece_id, int num_owned_piece) {
        logPrintf("Peer [%d] has downloaded the piece [%d] from [%d]. "
                  "Now the number of pieces it has is [%d].\n", self_peer_id, piece_id, peer_id,
                  num_owned_piece);
    }

    void fileDownloaded() {
        logPrintf("Peer [%d] has downloaded the complete file.\n", self_peer_id);
    }

    void sessionEnd(int peer_id) {
        logPrintf("SESSION END: (Self [%d] - Peer [%d])\n", self_peer_id, peer_id);
    }

    void tearDownReceived(int peer_id) {
        logPrintf("TearDown Received from peer [%d]\n", peer_id);
    }

    void sendingBcastTo(int peer_id, int piece_id) {
        logPrintf("Sending Bcast Have [piece id = %d] to peer [%d]\n", piece_id, peer_id);
    }

    void requestSentTo(int peer_id, int piece_id) {
        logPrintf("REQUEST SENT: from self [%d] to peer [%d], piece id = %d\n",
                  self_peer_id, peer_id, piece_id);
    }

    void requestRecvFrom(int peer_id, int piece_id) {
        logPrintf("REQUEST RECEIVED: from peer [%d] to self [%d], piece id = %d\n",
                  peer_id, self_peer_id, piece_id);
    }

    void pieceSentTo(int peer_id, int piece_id) {
        logPrintf("PIECE SENT: from self [%d] to peer [%d], piece id = %d\n",
                  self_peer_id, peer_id, piece_id);
    }

    void connTmpFailed(int peer_id) {
        logPrintf("Connection Temporary Failure: from self [%d] to peer [%d], retry later\n",
                  self_peer_id, peer_id);
    }

#undef logPrintf
};


#endif //CNT5106_V4_LOGGER_HPP
