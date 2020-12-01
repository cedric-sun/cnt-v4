// cesun, 11/26/20 12:36 AM.

#ifndef CNT5106_V4_SYNCPIECEBITFIELD_HPP
#define CNT5106_V4_SYNCPIECEBITFIELD_HPP

#include <mutex>
#include "PieceBitfield.hpp"
#include "PieceBitfieldSnapshot.hpp"

class SyncPieceBitfield : public PieceBitfield {
    friend std::vector<int> PieceBitfield::operator-(const SyncPieceBitfield &rhs) const;

private:
    mutable std::mutex m;
public:
    explicit SyncPieceBitfield(const int size, bool owningAllPiece)
            : PieceBitfield{size, owningAllPiece} {}

    //thread safe
    PieceBitfieldSnapshot snapshot() const {
        const std::lock_guard lg{m};
        // actually sending PieceStatus::Request doesn't matter; for sanity we filter it for now.
        std::vector<PieceStatus> tmp(sv.size());
        std::transform(sv.cbegin(), sv.cend(), tmp.begin(), [](const auto &e) {
            return e == PieceStatus::REQUESTED ? PieceStatus::ABSENT : e;
        });
        return PieceBitfieldSnapshot{std::move(tmp)};
    }

    //thread safe
    void setOwned(const int i) override {
        if (i < 0 || i >= sv.size())
            panic("setOwn index out of bound.");
        const std::lock_guard lg{m};
        if (sv[i] != PieceStatus::REQUESTED)
            panic("setting a un-requested slot to OWNED");
        sv[i] = PieceStatus::OWNED;
        n_owned++;
    }

    //thread safe
    void setRequested(const int i) {
        const std::lock_guard lg{m};
        if (i < 0 || i >= sv.size())
            panic("setRequested index out of bound.");
        if (sv[i] != PieceStatus::ABSENT)
            panic("setRequested on a non-ABSENT slot!");
        sv[i] = PieceStatus::REQUESTED;
    }

    //thread safe
    bool owningAll() const override {
        const std::lock_guard lg{m};
        return PieceBitfield::owningAll();
    }

    //thread safe
    bool isOwned(const int i) const {
        const std::lock_guard lg{m};
        return sv[i] == PieceStatus::OWNED;
    }
};


#endif //CNT5106_V4_SYNCPIECEBITFIELD_HPP
