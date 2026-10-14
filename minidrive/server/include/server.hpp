#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

namespace minidrive {

class Server {
public:
    Server(const std::string& root_path, std::uint16_t port);
    ~Server();

    void start();
    void stop();

private:
    std::string root_path_;
    std::uint16_t port_;
    int listen_fd_;
    bool running_;
    std::vector<std::thread> threads_;

    void accept_loop();
    void handle_client(int client_fd);
    std::string get_user_root(const std::string& username, bool is_public) const;
    std::string normalize_path(const std::string& base_dir, const std::string& path, const std::string& user_root) const;
    bool is_path_safe(const std::string& normalized_path, const std::string& user_root) const;
};

}
