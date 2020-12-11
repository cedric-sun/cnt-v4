// cesun, 11/26/20 2:14 AM.

#include "Connection.hpp"
#include "ConnectionError.hpp"
#include <netdb.h>

Connection::Connection(const std::string &fqdn, int port) {
    if (port < 0 || port >= 65536)
        panic("port out of range: " + std::to_string(port));
    // TODO: ensure aggregate init makes ai_flag be 0
    ::addrinfo hints{.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_protocol = 0};
    ::addrinfo *res;
    if (::getaddrinfo(fqdn.c_str(), nullptr, &hints, &res) != 0)
        panic("getaddrinfo"); // TODO: getaddrinfo do not use errno. see gai_strerror().
    reinterpret_cast<::sockaddr_in *>(res->ai_addr)->sin_port = htons(port);
    fd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        panic("socket");
    if (::connect(fd, res->ai_addr, res->ai_addrlen) == -1)
        throw ConnectionError{};
    ::freeaddrinfo(res); // TODO: fix leak
}

