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
        log_stream = stdout;//std::fopen(path.c_str(), "w");
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

    void newPreferredNeighbors(std::vector<int> peer_ids) {
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

    void fileDownloaded() {
        logPrintf("Peer [%d] has downloaded the complete file.\n", self_peer_id);
    }
#undef logPrintf
};


#endif //CNT5106_V4_LOGGER_HPP
