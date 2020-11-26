// cesun, 11/23/20 2:32 PM.

#ifndef CNT5106_V4_SESSIONCOLLECTION_HPP
#define CNT5106_V4_SESSIONCOLLECTION_HPP

#include "Session.hpp"
#include <vector>

class SessionCollection {
private:
    std::vector<Session> ss;
    std::mutex m;
public:
    void broadcastHave(const int i) {
        std::lock_guard lg{m};
        std::for_each(ss.begin(), ss.end(), [i](Session &s) { s.ackHave(i); });
    }

    void newSession();
};


#endif //CNT5106_V4_SESSIONCOLLECTION_HPP
