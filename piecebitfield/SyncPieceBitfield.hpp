// cesun, 11/26/20 12:36 AM.

#ifndef CNT5106_V4_SYNCPIECEBITFIELD_HPP
#define CNT5106_V4_SYNCPIECEBITFIELD_HPP

#include "AbstractPieceBitfield.hpp"
#include "PieceBitfieldSnapshot.hpp"
#include <mutex>
#include <atomic>

// TODO: refactor & optimize to be lock-free!
class SyncPieceBitfield : public AbstractPieceBitfield {
private:
    std::atomic_int n_owned;
    mutable std::mutex m;
public:
    void lock() const override {
        m.lock();
    }

    void unlock() const override {
        m.unlock();
    }

    bool try_lock() const override {
        return m.try_lock();
    }

public:
    explicit SyncPieceBitfield(const int size, bool owningAllPiece)
            : AbstractPieceBitfield{
            std::vector(size, owningAllPiece ? PieceStatus::OWNED : PieceStatus::ABSENT)} {}

    PieceBitfieldSnapshot snapshot() const {
        std::vector<PieceStatus> tmp(sv.size());
        const std::lock_guard lg{m};
        // actually sending PieceStatus::Request doesn't matter; for sanity we filter it for now.
        std::transform(sv.cbegin(), sv.cend(), tmp.begin(), [](const auto &e) {
            return e == PieceStatus::REQUESTED ? PieceStatus::ABSENT : e;
        });
        return PieceBitfieldSnapshot{std::move(tmp)};
    }

    bool isOwned(const int i) const override {
        checkRange(i);
        const std::lock_guard lg{m};
        return sv[i] == PieceStatus::OWNED;
    }

    void setOwned(const int i) override {
        checkRange(i);
        const std::lock_guard lg{m};
        if (sv[i] != PieceStatus::OWNED)
            n_owned++;
        sv[i] = PieceStatus::OWNED;
    }

    bool owningAll() const override {
        return static_cast<int>(n_owned) == sv.size();
    }

    int numOwned() const {
        return static_cast<int>(n_owned);
    }

    void setRequested(const int i) {
        checkRange(i);
        const std::lock_guard lg{m};
        sv[i] = PieceStatus::REQUESTED;
    }

    bool isRequested(const int i) {
        checkRange(i);
        const std::lock_guard lg{m};
        return sv[i] == PieceStatus::REQUESTED;
    }
};


#endif //CNT5106_V4_SYNCPIECEBITFIELD_HPP
