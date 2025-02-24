// cesun, 11/23/20 12:51 AM.

#ifndef CNT5106_V4_CONNECTION_HPP
#define CNT5106_V4_CONNECTION_HPP

#include "../io/IReader.hpp"
#include "../io/IWriter.hpp"
#include "../utils/class_utils.hpp"
#include "../utils/err_utils.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <utility>


class Connection : public IReader, public IWriter {
private:
    static constexpr int RUIN_FD = -1;
    int fd;
    std::atomic_int64_t recv_bcnt{0};
public:
    explicit Connection(int fd) : fd{fd} {}

    explicit Connection(const std::string &fqdn, int port);

    DISABLE_COPY(Connection)

    Connection(Connection &&other) noexcept: fd{std::exchange(other.fd, RUIN_FD)} {}

    void operator=(Connection &&) = delete;

    ~Connection() override {
        if (fd != RUIN_FD) {
            ::close(fd);
            fd = RUIN_FD;
        }
    }

    int read(void *buf, int length) override {
        int n = ::recv(fd, buf, length, 0);
        if (n == -1)
            panic("::recv() failure");
        if (n==0) {
            // peer closed its socket. The problem is that the spec doesn't have a sane
            // way to do connection tear-down. At this moment the session is about to end,
            // and this thread should soon receive a pthread_cancel; we sleep here to prevent
            // busy waiting.
//            std::puts("==============peer closed the socket.=================");
            ::sleep(1);
        }
        recv_bcnt += n;
        return n;
    }

    void write(const void *buf, int length) override {
        int n = ::send(fd, buf, length, 0);
        if (n != length)
            panic("::send() failure");
    }

    // thread-safe
    [[nodiscard]] int64_t receivedByteCount() const {
        return static_cast<int64_t>(recv_bcnt);
    }
};


#endif //CNT5106_V4_CONNECTION_HPP
