// cesun, 11/23/20 5:40 PM.

#ifndef CNT5106_V4_MSG_HPP
#define CNT5106_V4_MSG_HPP

#include "../io/BufferedWriter.hpp"

class Msg {
public:
    virtual void writeTo(BufferedWriter &w) const = 0;

    virtual ~Msg() = default;
};


#endif //CNT5106_V4_MSG_HPP
