// cesun, 11/28/20 3:33 AM.

#include "Config.hpp"
#include <fstream>
#include "utils/err_utils.hpp"
#include <unordered_set>
#include <filesystem>

Config::Config(int self_peer_id)
        : self_peer_id{self_peer_id}, peer_dir{std::to_string(self_peer_id)},
          log_filepath{"log_peer_" + std::to_string(self_peer_id) + ".log"} {
    std::ifstream common_ifs{"Common.cfg"};
    std::string key, value;
    try {
        while (common_ifs >> key >> value) {
            // switch can't work on std::string
            if (key == "NumberOfPreferredNeighbors") {
                num_pn = std::stoi(value);
            } else if (key == "UnchokingInterval") {
                pn_interval = std::stoi(value);
            } else if (key == "OptimisticUnchokingInterval") {
                opt_interval = std::stoi(value);
            } else if (key == "FileName") {
                file_path = std::filesystem::path{peer_dir} / value;
            } else if (key == "FileSize") {
                file_size = std::stoi(value);
            } else if (key == "PieceSize") {
                piece_size = std::stoi(value);
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
    bool self_found = false;
    std::unordered_set<decltype(peer_id)> idset;
    while (peerinfo_ifs >> peer_id >> fqdn >> port >> has_file) {
        if (auto it = idset.find(peer_id);it != idset.cend()) {
            panic("duplicated peer id appeared in PeerInfo.cfg");
        }
        idset.insert(peer_id);
        //TODO ensure the same (hostname, port) pair does not appear multiple time
        if (!self_found && peer_id == self_peer_id) {
            this->has_file = static_cast<bool>(has_file);
            this->self_port = port;
            self_found = true;
        }
        if (!self_found)
            prior_peers.emplace_back(peer_id, port, fqdn);
    }
    if (!self_found)
        panic("can't find self (peer id " + std::to_string(self_peer_id) + " ) inside Common.cfg");
    n_total_peer = idset.size();
}
