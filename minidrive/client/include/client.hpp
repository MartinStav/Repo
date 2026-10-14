#pragma once

#include <string>
#include <cstdint>

namespace minidrive {

class Client {
public:
    Client(const std::string& host, std::uint16_t port, const std::string& username = "", const std::string& log_file = "");
    ~Client();

    bool connect();
    void disconnect();
    void run();

private:
    std::string host_;
    std::uint16_t port_;
    std::string username_;
    std::string log_file_;
    int fd_;
    bool connected_;
};

}
