// cesun, 11/23/20 5:42 PM.

#include "PieceRepository.hpp"
#include <mutex>
#include "File.hpp"

// precondition:
void PieceRepository::ensureSpace(const int64_t size) {
    while (n_cached_byte + size > kMemLimit) {
        auto cit = cache.cbegin();
        n_cached_byte -= cit->second->size();
        cache.erase(cit);
    }
}

std::shared_ptr<Piece> PieceRepository::get(const int i) {
    const std::lock_guard lg(spbf); // TODO use reader writer lock
    if (auto it = cache.find(i); it != cache.cend()) {
        return it->second;
    } else {
        auto begin = i * piece_size;
        auto size = std::min(piece_size, f->size() - begin);
        std::vector<Byte> bytes(size);
        f->readAt(begin, bytes.data(), size); // heavy IO
        ensureSpace(size);
        auto piece_sp = std::make_shared<Piece>(std::move(bytes));
        cache.emplace(i, piece_sp);
        return piece_sp; // implicitly moved
    }
}

void PieceRepository::save(int i, std::shared_ptr<Piece> piece_up) {
    const std::lock_guard lg(spbf);
    auto begin = i * piece_size;
    auto expected_size = std::min(piece_size, f->size() - begin);
    if (piece_up->size() != expected_size)
        panic("unexpected piece size");
    f->writeAt(begin, piece_up->data(), piece_up->size());

    ensureSpace(piece_up->size());
    cache.emplace(i, std::shared_ptr<Piece>{std::move(piece_up)});
}
