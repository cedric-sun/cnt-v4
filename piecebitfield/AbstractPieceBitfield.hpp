// cesun, 12/1/20 1:01 PM.

#ifndef CNT5106_V4_ABSTRACTPIECEBITFIELD_HPP
#define CNT5106_V4_ABSTRACTPIECEBITFIELD_HPP

#include "PieceStatus.hpp"
#include <vector>

class AbstractPieceBitfield {
private:
    virtual void lock() const = 0;

    virtual void unlock() const = 0;

    virtual bool try_lock() const = 0;

protected:
    std::vector<PieceStatus> sv;

    explicit AbstractPieceBitfield(std::vector<PieceStatus> sv) : sv{std::move(sv)} {}

    void checkRange(const int i) const {
        if (i < 0 || i >= sv.size())
            panic("index out of bound.");
    }

public:
    std::vector<int> operator-(const AbstractPieceBitfield &rhs) const;

    virtual bool isOwned(const int i) const = 0;

    virtual void setOwned(const int i) = 0;

    [[nodiscard]] virtual bool owningAll() const = 0;
};


#endif //CNT5106_V4_ABSTRACTPIECEBITFIELD_HPP
