// cesun, 11/23/20 12:51 AM.

#ifndef CNT5106_V4_CONNECTION_HPP
#define CNT5106_V4_CONNECTION_HPP

#include "../io/IReader.hpp"
#include "../io/IWriter.hpp"
#include "../utils/class_utils.hpp"
#include "../utils/err_utils.hpp"
#include <sys/socket.h>
#include <utility>
#include <atomic>
#include <cstdint>

class Connection : public IReader, public IWriter {
private:
    static const int RUIN_FD = -1;
    int fd;
    std::atomic_int64_t recv_bcnt{0};
public:
    explicit Connection(int fd) : fd{fd} {}

    explicit Connection(const std::string &fqdn, int port);

    DISABLE_COPY(Connection)

    Connection(Connection &&other) noexcept: fd{std::exchange(other.fd, RUIN_FD)} {}

    void operator=(Connection &&) = delete;

    ~Connection() override { close(); }

    int read(void *buf, int length) override {
        int n = ::recv(fd, buf, length, 0);
        if (n == -1)
            panic("::recv() failure");
        recv_bcnt += n;
        return n;
    }

    void write(const void *buf, int length) override {
        int n = ::send(fd, buf, length, 0);
        if (n != length)
            panic("::send() failure");
    }

    void close() {
        if (fd != RUIN_FD) {
            ::close(fd);
            fd = RUIN_FD;
        }
    }

    // thread-safe
    [[nodiscard]] int64_t receivedByteCount() const {
        return static_cast<int64_t>(recv_bcnt);
    }
};


#endif //CNT5106_V4_CONNECTION_HPP
