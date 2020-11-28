// cesun, 11/28/20 3:33 AM.

#ifndef CNT5106_V4_CONFIG_HPP
#define CNT5106_V4_CONFIG_HPP

#include <string>
#include <vector>

class PeerInfo {
public:
    const int peer_id, port;
    const std::string fqdn;

    PeerInfo(const int peer_id, const int port, std::string fqdn)
            : peer_id(peer_id), port(port), fqdn(std::move(fqdn)) {}
};


class Config {
private:
    int num_pn, pn_interval, opt_interval, piece_size, file_size;
    const int self_peer_id;
    const std::string log_filepath;
    int self_port;
    bool has_file;
    std::string file_path;
    std::vector<PeerInfo> prior_peers;

public:
    explicit Config(int self_peer_id);

    [[nodiscard]] int getFileSize() const {
        return file_size;
    }

    [[nodiscard]] bool hasFile() const {
        return has_file;
    }

    [[nodiscard]] int getNumPreferredNeighbors() const {
        return num_pn;
    }

    [[nodiscard]] int getUnchokingInterval() const {
        return pn_interval;
    }

    [[nodiscard]] int getOptUnchokingInterval() const {
        return opt_interval;
    }

    [[nodiscard]]  int getPieceSize() const {
        return piece_size;
    }

    [[nodiscard]]  const int getSelfPeerId() const {
        return self_peer_id;
    }

    // "log_peer_xxxx.log"
    [[nodiscard]]   const std::string &getLogFilepath() const {
        return log_filepath;
    }

    [[nodiscard]]  int getPort() const {
        return self_port;
    }

    // "peer_xxxx/THE_FILE"
    [[nodiscard]]  const std::string &getFilePath() const {
        return file_path;
    }

    [[nodiscard]]  const std::vector<PeerInfo> &getPriorPeers() const {
        return prior_peers;
    }
};


#endif //CNT5106_V4_CONFIG_HPP
