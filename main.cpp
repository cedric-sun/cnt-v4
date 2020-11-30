#include <cstdio>

#include "storage/NewFile.hpp"
#include "storage/ExistingFile.hpp"
#include "storage/PieceRepository.hpp"
#include "session/SessionCollection.hpp"
#include "tcp/Acceptor.hpp"
#include "Config.hpp"
#include "Logger.hpp"

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
        std::filesystem::create_directory(config.getFilePath(), ec);
        if (ec)
            panic("std::filesystem::create_directory");
    }
}

void startServer(const int port, SessionCollection &sc) {
    std::thread{[&] {
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
    std::unique_ptr<File> file_up{nullptr};
    std::optional<SyncPieceBitfield> sopt{std::nullopt};
    // chaotic evil: bypass the scope rule
    // SyncPieceBitfield foo = config.hasFile()?SyncPieceBitfield{}:SyncPieceBitfield{};
    if (config.hasFile()) {
        file_up = std::make_unique<ExistingFile>(config.getFilePath());
        sopt.emplace();
    } else {
        file_up = std::make_unique<NewFile>(config.getFilePath(), config.getFileSize());
        sopt.emplace();
    }
    PieceRepository repo{std::move(file_up)};
    SessionCollection sc{};
    startServer(config.getPort(), sc);
    for (const auto &pi : config.getPriorPeers()) {
        sc.newSession(Connection{pi.fqdn, pi.port}, pi.peer_id);
    }
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    sc.wait();
    return 0;
}
