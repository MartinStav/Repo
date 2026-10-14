#include "minidrive/protocol.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdint>

namespace minidrive {

std::string read_message(int fd) {
    uint32_t len;
    ssize_t n = recv(fd, &len, sizeof(len), MSG_WAITALL);
    if (n != sizeof(len)) {
        return "";
    }
    len = ntohl(len);
    if (len > 10 * 1024 * 1024) {
        return "";
    }
    std::string msg(len, '\0');
    n = recv(fd, msg.data(), len, MSG_WAITALL);
    if (n != static_cast<ssize_t>(len)) {
        return "";
    }
    return msg;
}

void write_message(int fd, const std::string& msg) {
    uint32_t len = htonl(static_cast<uint32_t>(msg.size()));
    ssize_t sent = send(fd, &len, sizeof(len), 0);
    if (sent != sizeof(len)) {
        return;
    }

    size_t total_sent = 0;
    while (total_sent < msg.size()) {
        ssize_t n = send(fd, msg.data() + total_sent, msg.size() - total_sent, 0);
        if (n <= 0) {
            break;
        }
        total_sent += static_cast<size_t>(n);
    }
}

}
