// cesun, 11/26/20 2:20 AM.

#include "Acceptor.hpp"

#include <arpa/inet.h>

Acceptor::Acceptor(int port) {
    if (port < 0 || port > UINT16_MAX)
        panic("illegal port number:" + std::to_string(port));
    server_socket_fd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
        panic("socket");
    ::sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = ::htons(static_cast<uint16_t>(port)),
    };
    addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    const int REUSEADDR_TRUE = 1;
    ::setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR,
                 &REUSEADDR_TRUE, sizeof(REUSEADDR_TRUE));
    if (::bind(server_socket_fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) == -1)
        panic("bind");
    if (::listen(server_socket_fd, ACCEPT_QUEUE_SIZE) == -1)
        panic("listen");
}
