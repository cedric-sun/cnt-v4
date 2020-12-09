#include <cstdio>

#include "storage/NewFile.hpp"
#include "storage/ExistingFile.hpp"
#include "storage/PieceRepository.hpp"
#include "session/SessionCollection.hpp"
#include "tcp/Acceptor.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "piecebitfield/SyncPieceBitfield.hpp"
#include "utils/MathUtils.hpp"
#include "tcp/ConnectionError.hpp"

[[noreturn]] static void printUsageAndExit(char *argv0) {
    fprintf(stderr, "Usage: %s <peerID>\n", argv0);
    exit(EXIT_FAILURE);
}

static int parseArgs(int argc, char **argv) {
    if (argc != 2)
        printUsageAndExit(argv[0]);
    int my_peer_id;
    try {
        my_peer_id = std::stoi(argv[1]);
    } catch (...) {
        printUsageAndExit(argv[0]);
    }
    return my_peer_id;
}

static void validateConfig(const Config &config) {
    if (config.hasFile()) {
        if (!std::filesystem::exists(config.getFilePath()))
            panic("self is supposed to own the file, but file is not found.");
        if (std::filesystem::file_size(config.getFilePath()) != config.getFileSize())
            panic("file length specified in config does not match"
                  " real file length on filesystem.");
    } else {
        // ensure the existence of the peer dir
        std::error_code ec;
        // existing directory is not an error
        std::filesystem::create_directory(config.getPeerDir(), ec);
        if (ec)
            panic("std::filesystem::create_directory");
    }
}

void startServer(const int port, SessionCollection &sc) {
    std::thread{[port, &sc] {
        Acceptor acc{port};
        while (true) {
            sc.newSession(acc.accept(), Session::EPID_NO_PREFERENCE);
        }
    }}.detach();
}

int main(int argc, char **argv) {
    const int self_peer_id = parseArgs(argc, argv);
    Config config{self_peer_id};
    validateConfig(config);
    Logger logger{self_peer_id, config.getLogFilepath()};
    std::unique_ptr<File> file_up{nullptr};
    if (config.hasFile()) {
        file_up = std::make_unique<ExistingFile>(config.getFilePath());
    } else {
        file_up = std::make_unique<NewFile>(config.getFilePath(), config.getFileSize());
    }
    SyncPieceBitfield spbf{MathUtils::ceilingDiv(config.getFileSize(), config.getPieceSize()),
                           config.hasFile()};
    PieceRepository repo{std::move(file_up), config.getPieceSize()};
    SessionCollection sc{config.totalPeerCount() - 1, config.getUnchokingInterval(),
                         config.getOptUnchokingInterval(), config.getNumPreferredNeighbors(),
                         self_peer_id, spbf, repo, logger};
    startServer(config.getPort(), sc);
    {
        const auto &prior_peers = config.getPriorPeers();
        int n_unsucc_conn = prior_peers.size();
        std::vector<bool> conn_succ(n_unsucc_conn, false);
        while (n_unsucc_conn > 0) {
            for (int i = 0; i < conn_succ.size(); ++i) {
                if (!conn_succ[i]) {
                    const auto &peer = prior_peers[i];
                    try {
                        sc.newSession(Connection{peer.fqdn, peer.port}, peer.peer_id);
                    } catch (ConnectionError &) {
                        continue;
                    }
                    conn_succ[i] = true;
                    n_unsucc_conn--;
                }
            }
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        }
    }
    sc.wait();
    return 0;
}
