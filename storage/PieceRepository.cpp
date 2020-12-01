// cesun, 11/23/20 5:42 PM.

#include "PieceRepository.hpp"
#include "../piecebitfield/SyncPieceBitfield.hpp"
#include <mutex>
#include "File.hpp"

// precondition: m_cache exclusively acquired
void PieceRepository::ensureSpace(const int64_t size) {
    while (n_cached_byte + size > kMemLimit) { //TODO: improve swap out algorithm
        auto cit = cache.cbegin();
        n_cached_byte -= cit->second->size();
        cache.erase(cit);
    }
}

std::shared_ptr<Piece> PieceRepository::get(const int i) {
    if (!spbf.isOwned(i))
        panic("receive a request for a piece that self does not own");
    {
        const std::shared_lock reader_lock(m_cache);
        if (auto it = cache.find(i); it != cache.cend()) {
            return it->second;
        }
    }
    auto begin = i * piece_size;
    auto size = std::min(piece_size, f->size() - begin);
    std::vector<Byte> bytes(size);
    f->readAt(begin, bytes.data(), size); // heavy IO
    auto piece_sp = std::make_shared<Piece>(std::move(bytes));
    {
        const std::lock_guard writer_lock(m_cache);
        ensureSpace(size);
        cache.emplace(i, piece_sp);
    }
    return piece_sp; // implicitly moved
}

//TODO
//      session depends on both SyncPieceBitfield and PieceRepository, where shall
//          we do the error handling. (don't do it twice!)
//          SyncPieceBitfield is assuming too much responsibility for error checking now.
void PieceRepository::save(int i, std::shared_ptr<Piece> piece_up) {
    spbf.setOwned(i);
    auto begin = i * piece_size;
    auto expected_size = std::min(piece_size, f->size() - begin);
    if (piece_up->size() != expected_size)
        panic("unexpected piece size");
    f->writeAt(begin, piece_up->data(), piece_up->size());
    {
        const std::lock_guard lg(spbf);
        ensureSpace(piece_up->size());
        cache.emplace(i, std::shared_ptr<Piece>{std::move(piece_up)});
    }
}
