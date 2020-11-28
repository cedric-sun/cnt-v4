// cesun, 11/26/20 12:36 AM.

#ifndef CNT5106_V4_SYNCPIECEBITFIELD_HPP
#define CNT5106_V4_SYNCPIECEBITFIELD_HPP

#include <mutex>
#include "PieceBitfield.hpp"
#include "SimplePieceBitfield.hpp"

class SyncPieceBitfield : public PieceBitfield {
private:
    mutable std::mutex m;
public:
    void lock() const { m.lock(); }

    void unlock() const { m.unlock(); }

    bool try_lock() const { return m.try_lock(); }
};


#endif //CNT5106_V4_SYNCPIECEBITFIELD_HPP
