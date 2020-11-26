// cesun, 11/26/20 2:20 AM.

#ifndef CNT5106_V4_ACCEPTOR_HPP
#define CNT5106_V4_ACCEPTOR_HPP

#include "../utils/class_utils.hpp"
#include "Connection.hpp"
#include <unistd.h>

class Acceptor {
private:
    int server_socket_fd;
public:
    explicit Acceptor(int port);
    DISABLE_COPY_MOVE(Acceptor)

    ~Acceptor() { close(server_socket_fd); }

    [[nodiscard]] Connection accept() const {
        int client_socket_fd = ::accept(server_socket_fd, nullptr, nullptr);
        if (client_socket_fd == -1)
            panic("accept() failed");
        return Connection{client_socket_fd};
    }
};


#endif //CNT5106_V4_ACCEPTOR_HPP
