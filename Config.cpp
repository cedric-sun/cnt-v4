// cesun, 11/28/20 3:33 AM.

#include "Config.hpp"
#include <fstream>
#include "utils/err_utils.hpp"

Config::Config(int self_peer_id)
        : self_peer_id{self_peer_id},
          log_filepath{"log_peer_" + std::to_string(self_peer_id) + ".log"} {
    std::ifstream common_ifs{"Common.cfg"};
    std::string key, value;
    try {
        while (common_ifs >> key >> value) {
            // switch can't work on std::string
            if (key == "NumberOfPreferredNeighbors") {
                num_pn = std::stoi(key);
            } else if (key == "UnchokingInterval") {
                pn_interval = std::stoi(key);
            } else if (key == "OptimisticUnchokingInterval") {
                opt_interval = std::stoi(key);
            } else if (key == "FileName") {
                file_path = key;
            } else if (key == "FileSize") {
                file_size = std::stoi(key);
            } else if (key == "PieceSize") {
                piece_size = std::stoi(key);
            } else {
                panic("unknown key name in Common.cfg");
            }
        }
    } catch (...) {
        panic("Fail to parse integer in Common.cfg");
    }
    std::ifstream peerinfo_ifs{"PeerInfo.cfg"};
    int peer_id, port, has_file;
    std::string fqdn;
    while (peerinfo_ifs >> peer_id >> fqdn >> port >> has_file) {
        if (peer_id == self_peer_id) {
            this->has_file = has_file;
            this->self_port = port;
            break;
        }
        prior_peers.emplace_back(peer_id, port, fqdn);
    }
}
