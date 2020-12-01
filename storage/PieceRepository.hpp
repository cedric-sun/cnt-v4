// cesun, 11/23/20 5:42 PM.

#ifndef CNT5106_V4_PIECEREPOSITORY_HPP
#define CNT5106_V4_PIECEREPOSITORY_HPP

#include "Piece.hpp"
#include <memory>
#include <unordered_map>
#include <shared_mutex>

class File;

class PieceRepository {
private:
    static const int kMemLimit = 16 * 1024 * 1024; // 16 MiB piece cache

    std::unique_ptr<File> f;
    const int64_t piece_size;
    std::unordered_map<int, std::shared_ptr<Piece>> cache;
    std::shared_mutex m_cache;
    int64_t n_cached_byte{0};

    void ensureSpace(int64_t size);

public:
    explicit PieceRepository(std::unique_ptr<File> f, const int64_t piece_size)
            : f{std::move(f)}, piece_size{piece_size} {}

    std::shared_ptr<Piece> get(int i);

    void save(int i, std::shared_ptr<Piece> piece_up);
};


#endif //CNT5106_V4_PIECEREPOSITORY_HPP
