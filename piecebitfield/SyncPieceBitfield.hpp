// cesun, 11/26/20 12:36 AM.

#ifndef CNT5106_V4_SYNCPIECEBITFIELD_HPP
#define CNT5106_V4_SYNCPIECEBITFIELD_HPP

#include <mutex>
#include "PieceBitfield.hpp"
#include "PieceBitfieldSnapshot.hpp"

class SyncPieceBitfield : public PieceBitfield {
private:
    mutable std::mutex m;
public:
    explicit SyncPieceBitfield(const int size, bool owningAllPiece)
            : PieceBitfield{size, owningAllPiece} {}

    PieceBitfieldSnapshot snapshot() const {
        const std::lock_guard lg{m};
        // potentially sending REQUEST as well, but it doesn't matter
        return PieceBitfieldSnapshot{sv}; // copy
    }

    void setOwned(const int i) override {
        const std::lock_guard lg{m};
        PieceBitfield::setOwned(i);
    }

    bool owningAll() const override {
        const std::lock_guard lg{m};
        return PieceBitfield::owningAll();
    }


    void lock() const { m.lock(); }

    void unlock() const { m.unlock(); }

    bool try_lock() const { return m.try_lock(); }
};


#endif //CNT5106_V4_SYNCPIECEBITFIELD_HPP
