#include "server.hpp"
#include "minidrive/protocol.hpp"
#include "minidrive/user_db.hpp"
#include "minidrive/crypto.hpp"

#include <spdlog/spdlog.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <filesystem>
#include <fstream>
#include <cerrno>
#include <cstring>

namespace minidrive {

namespace {
    Server* g_server = nullptr;

    void signal_handler(int sig) {
        if (g_server && sig == SIGTERM) {
            g_server->stop();
        }
    }
}

Server::Server(const std::string& root_path, std::uint16_t port)
    : root_path_(root_path), port_(port), listen_fd_(-1), running_(false) {
    std::filesystem::create_directories(root_path_);
    std::filesystem::create_directories(root_path_ + "/public");
    std::filesystem::create_directories(root_path_ + "/users");

    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path_ + "/users")) {
            if (entry.is_regular_file() && entry.path().extension() == ".part") {
                auto mod_time = std::filesystem::last_write_time(entry.path());
                auto now = std::filesystem::file_time_type::clock::now();
                auto age = now - mod_time;
                if (age > std::chrono::hours(1)) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    } catch (...) {
    }

    g_server = this;
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
}

Server::~Server() {
    stop();
    if (listen_fd_ >= 0) {
        ::close(listen_fd_);
    }
}

void Server::start() {
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        std::perror("socket");
        return;
    }

    int enable = 1;
    ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);

    if (::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("bind");
        return;
    }

    if (::listen(listen_fd_, 128) < 0) {
        std::perror("listen");
        return;
    }

    spdlog::info("Server listening on port {}", port_);
    running_ = true;
    accept_loop();
}

void Server::stop() {
    running_ = false;
    if (listen_fd_ >= 0) {
        ::shutdown(listen_fd_, SHUT_RDWR);
    }
    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    threads_.clear();
}

void Server::accept_loop() {
    while (running_) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = ::accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd < 0) {
            if (errno == EINTR || errno == EAGAIN) continue;
            if (running_) std::perror("accept");
            break;
        }

        threads_.emplace_back([this, client_fd]() {
            handle_client(client_fd);
        });
    }
}

std::string Server::get_user_root(const std::string& username, bool is_public) const {
    if (is_public) {
        return root_path_ + "/public";
    }
    return root_path_ + "/users/" + username;
}

std::string Server::normalize_path(const std::string& base_dir, const std::string& path, const std::string& user_root) const {
    std::string normalized;
    if (path.empty() || path == ".") {
        normalized = base_dir;
    } else if (path[0] == '/') {
        normalized = user_root + path;
    } else {
        normalized = base_dir + "/" + path;
    }
    std::filesystem::path p(normalized);
    p = p.lexically_normal();
    return p.string();
}

bool Server::is_path_safe(const std::string& normalized_path, const std::string& user_root) const {
    try {
        std::filesystem::path p = normalized_path;
        std::filesystem::path root = std::filesystem::canonical(user_root);

        std::filesystem::path existing = p;
        while (!std::filesystem::exists(existing) && existing.has_parent_path()) {
            existing = existing.parent_path();
        }

        if (std::filesystem::exists(existing)) {
            std::filesystem::path canon = std::filesystem::canonical(existing);
            std::string canon_str = canon.string();
            std::string root_str = root.string();

            if (canon_str.find(root_str) != 0) {
                return false;
            }
        }

        std::string rel = std::filesystem::relative(p, root).string();
        if (rel.find("..") == 0) {
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

void Server::handle_client(int client_fd) {
    UserDB user_db(root_path_ + "/users.db");
    std::string username;
    bool is_public = true;
    std::string user_root;

    try {
        std::string auth_msg = read_message(client_fd);
        if (auth_msg.empty()) {
            ::close(client_fd);
            return;
        }

        auto auth_json = nlohmann::json::parse(auth_msg);
        std::string auth_type = auth_json.value("type", "");

        if (auth_type == "auth") {
            username = auth_json.value("username", "");
            std::string password = auth_json.value("password", "");

            if (user_db.user_exists(username)) {
                if (user_db.authenticate(username, password)) {
                    is_public = false;
                    user_root = get_user_root(username, false);
                    std::filesystem::create_directories(user_root);

                    spdlog::info("User {} logged in", username);

                    Response r = Response::ok({{"message", "Logged as " + username}});
                    write_message(client_fd, r.to_json().dump());
                } else {
                    Response r = Response::error(101, "Invalid password");
                    write_message(client_fd, r.to_json().dump());
                    ::close(client_fd);
                    return;
                }
            } else {
                Response r = Response::error(102, "User not found");
                write_message(client_fd, r.to_json().dump());

                std::string reg_msg = read_message(client_fd);
                if (reg_msg.empty()) {
                    ::close(client_fd);
                    return;
                }

                auto reg_json = nlohmann::json::parse(reg_msg);
                std::string reg_type = reg_json.value("type", "");

                if (reg_type == "register") {
                    std::string reg_username = reg_json.value("username", "");
                    std::string reg_password = reg_json.value("password", "");

                    if (user_db.register_user(reg_username, reg_password)) {
                        spdlog::info("User {} registered", reg_username);
                        Response reg_r = Response::ok({{"message", "User registered successfully"}});
                        write_message(client_fd, reg_r.to_json().dump());
                    } else {
                        spdlog::error("Registration failed for {}", reg_username);
                        Response reg_r = Response::error(104, "Registration failed");
                        write_message(client_fd, reg_r.to_json().dump());
                    }
                }
                ::close(client_fd);
                return;
            }
        } else if (auth_type == "register") {
            username = auth_json.value("username", "");
            std::string password = auth_json.value("password", "");

            if (user_db.user_exists(username)) {
                Response r = Response::error(103, "User already exists");
                write_message(client_fd, r.to_json().dump());
                ::close(client_fd);
                return;
            }

            if (user_db.register_user(username, password)) {
                Response r = Response::ok({{"message", "User registered successfully"}});
                write_message(client_fd, r.to_json().dump());
            } else {
                Response r = Response::error(104, "Registration failed");
                write_message(client_fd, r.to_json().dump());
            }
            ::close(client_fd);
            return;
        } else {
            is_public = true;
            user_root = get_user_root("", true);
            Response r = Response::ok({{"message", "Public mode"}});
            write_message(client_fd, r.to_json().dump());
        }

        std::string current_dir = user_root;

        while (true) {
            std::string msg = read_message(client_fd);
            if (msg.empty()) break;

            try {
                auto json = nlohmann::json::parse(msg);
                Request req = Request::from_json(json);
                Response resp;

                spdlog::info("Received command: {}, args: {}", req.cmd, req.args.dump());

                if (req.cmd == "LIST") {
                    std::string path = req.args.value("path", "");
                    std::string target_path = path.empty() ? current_dir : normalize_path(current_dir, path, user_root);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(target_path)) {
                        resp = Response::error(201, "Directory not found");
                    } else if (!std::filesystem::is_directory(target_path)) {
                        resp = Response::error(201, "Path is not a directory");
                    } else {
                        nlohmann::json entries = nlohmann::json::array();
                        for (const auto& entry : std::filesystem::directory_iterator(target_path)) {
                            nlohmann::json e;
                            e["name"] = entry.path().filename().string();
                            e["type"] = entry.is_directory() ? "dir" : "file";
                            if (entry.is_regular_file()) {
                                e["size"] = std::filesystem::file_size(entry.path());
                            }
                            entries.push_back(e);
                        }
                        resp = Response::ok({{"entries", entries}});
                    }
                    write_message(client_fd, resp.to_json().dump());
                    continue;
                }
                else if (req.cmd == "CD") {
                    std::string path = req.args.value("path", "");
                    std::string target_path;
                    if (path.empty() || path == ".") {
                        target_path = user_root;
                    } else if (path[0] == '/') {
                        target_path = normalize_path(user_root, path, user_root);
                    } else {
                        target_path = normalize_path(current_dir, path, user_root);
                    }

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(target_path) || !std::filesystem::is_directory(target_path)) {
                        resp = Response::error(201, "Directory not found");
                    } else {
                        current_dir = target_path;
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "MKDIR") {
                    std::string path = req.args.value("path", "");
                    std::string target_path = normalize_path(current_dir, path, user_root);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (std::filesystem::exists(target_path)) {
                        resp = Response::error(202, "Directory already exists");
                    } else {
                        std::filesystem::create_directories(target_path);
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "RMDIR") {
                    std::string path = req.args.value("path", "");
                    std::string target_path = normalize_path(current_dir, path, user_root);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(target_path) || !std::filesystem::is_directory(target_path)) {
                        resp = Response::error(201, "Directory not found");
                    } else {
                        std::filesystem::remove_all(target_path);
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "DELETE") {
                    std::string path = req.args.value("path", "");
                    std::string target_path = normalize_path(current_dir, path, user_root);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(target_path)) {
                        resp = Response::error(203, "File not found");
                    } else {
                        std::filesystem::remove(target_path);
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "MOVE") {
                    std::string src = req.args.value("src", "");
                    std::string dst = req.args.value("dst", "");
                    std::string src_path = normalize_path(current_dir, src, user_root);
                    std::string dst_path = normalize_path(current_dir, dst, user_root);

                    if (!is_path_safe(src_path, user_root) || !is_path_safe(dst_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(src_path)) {
                        resp = Response::error(203, "Source not found");
                    } else if (std::filesystem::exists(dst_path)) {
                        resp = Response::error(202, "Destination already exists");
                    } else {
                        std::filesystem::rename(src_path, dst_path);
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "COPY") {
                    std::string src = req.args.value("src", "");
                    std::string dst = req.args.value("dst", "");
                    std::string src_path = normalize_path(current_dir, src, user_root);
                    std::string dst_path = normalize_path(current_dir, dst, user_root);

                    if (!is_path_safe(src_path, user_root) || !is_path_safe(dst_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else if (!std::filesystem::exists(src_path)) {
                        resp = Response::error(203, "Source not found");
                    } else if (std::filesystem::exists(dst_path)) {
                        resp = Response::error(202, "Destination already exists");
                    } else {
                        if (std::filesystem::is_directory(src_path)) {
                            std::filesystem::copy(src_path, dst_path, std::filesystem::copy_options::recursive);
                        } else {
                            std::filesystem::copy_file(src_path, dst_path);
                        }
                        resp = Response::ok();
                    }
                }
                else if (req.cmd == "RESUME_UPLOAD_CHECK") {
                    if (is_public) {
                        resp = Response::error(208, "Resume only available in private mode");
                    } else {
                        std::string remote_path = req.args.value("remote_path", "");
                        std::string target_path = normalize_path(current_dir, remote_path, user_root);
                        std::string part_path = target_path + ".part";

                        if (!is_path_safe(target_path, user_root)) {
                            resp = Response::error(200, "Path traversal not allowed");
                        } else if (std::filesystem::exists(part_path)) {
                            uint64_t part_size = std::filesystem::file_size(part_path);
                            resp = Response::ok({{"uploaded_bytes", part_size}});
                        } else {
                            resp = Response::ok({{"uploaded_bytes", 0}});
                        }
                    }
                }
                else if (req.cmd == "RESUME_UPLOAD") {
                    if (is_public) {
                        resp = Response::error(208, "Resume only available in private mode");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    std::string remote_path = req.args.value("remote_path", "");
                    uint64_t file_size = req.args.value("file_size", 0ULL);
                    uint64_t start_offset = req.args.value("start_offset", 0ULL);
                    std::string file_hash = req.args.value("file_hash", "");

                    if (file_size > 4ULL * 1024 * 1024 * 1024) {
                        resp = Response::error(204, "File too large");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    std::string target_path = normalize_path(current_dir, remote_path, user_root);
                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    std::string part_path = target_path + ".part";
                    std::ios_base::openmode mode = (start_offset == 0) ? std::ios::binary : (std::ios::binary | std::ios::app);
                    std::ofstream file(part_path, mode);
                    if (!file) {
                        resp = Response::error(205, "Cannot create file");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    if (start_offset > 0) {
                        file.seekp(static_cast<std::streamoff>(start_offset));
                    }

                    resp = Response::ok();
                    write_message(client_fd, resp.to_json().dump());

                    uint64_t remaining = file_size - start_offset;
                    char buffer[65536];
                    while (remaining > 0) {
                        size_t to_read = std::min(remaining, static_cast<uint64_t>(sizeof(buffer)));
                        ssize_t n = recv(client_fd, buffer, to_read, 0);
                        if (n <= 0) {
                            file.close();
                            break;
                        }
                        file.write(buffer, n);
                        remaining -= static_cast<uint64_t>(n);
                    }
                    file.close();

                    if (remaining == 0) {
                        std::string computed_hash = hash_file(part_path);
                        if (computed_hash == file_hash) {
                            std::filesystem::rename(part_path, target_path);
                            resp = Response::ok();
                        } else {
                            resp = Response::error(206, "Hash mismatch");
                            std::filesystem::remove(part_path);
                        }
                    } else {
                        resp = Response::error(207, "Transfer incomplete");
                    }
                    write_message(client_fd, resp.to_json().dump());
                    continue;
                }
                else if (req.cmd == "UPLOAD") {
                    std::string remote_path = req.args.value("remote_path", "");
                    uint64_t file_size = req.args.value("file_size", 0ULL);
                    std::string file_hash = req.args.value("file_hash", "");

                    if (file_size > 4ULL * 1024 * 1024 * 1024) {
                        resp = Response::error(204, "File too large");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    std::string target_path = normalize_path(current_dir, remote_path, user_root);
                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    std::filesystem::path target_p(target_path);
                    std::filesystem::path parent_dir = target_p.parent_path();
                    if (!parent_dir.empty() && !std::filesystem::exists(parent_dir)) {
                        std::filesystem::create_directories(parent_dir);
                    }

                    if (std::filesystem::exists(target_path)) {
                        if (std::filesystem::is_directory(target_path)) {
                            resp = Response::error(202, "Path is a directory, not a file");
                        } else if (std::filesystem::is_regular_file(target_path)) {
                            resp = Response::error(202, "File already exists");
                        } else {
                            resp = Response::error(202, "Path already exists");
                        }
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    resp = Response::ok();
                    write_message(client_fd, resp.to_json().dump());

                    std::ofstream file(target_path, std::ios::binary);
                    if (!file) {
                        resp = Response::error(205, "Cannot create file");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    }

                    uint64_t remaining = file_size;
                    char buffer[65536];
                    while (remaining > 0) {
                        size_t to_read = std::min(remaining, static_cast<uint64_t>(sizeof(buffer)));
                        ssize_t n = recv(client_fd, buffer, to_read, 0);
                        if (n <= 0) {
                            file.close();
                            if (!is_public) {
                                std::string part_path = target_path + ".part";
                                std::filesystem::rename(target_path, part_path);
                            } else {
                                std::filesystem::remove(target_path);
                            }
                            break;
                        }
                        file.write(buffer, n);
                        remaining -= static_cast<uint64_t>(n);
                    }
                    file.flush();
                    file.close();

                    if (remaining == 0) {
                        std::string computed_hash = hash_file(target_path);
                        if (computed_hash == file_hash) {
                            resp = Response::ok();
                        } else {
                            resp = Response::error(206, "Hash mismatch");
                            std::filesystem::remove(target_path);
                        }
                        write_message(client_fd, resp.to_json().dump());
                    } else {
                        if (!is_public) {
                            std::string part_path = target_path + ".part";
                            std::filesystem::rename(target_path, part_path);
                            resp = Response::error(207, "Transfer incomplete");
                        } else {
                            resp = Response::error(207, "Transfer incomplete");
                            std::filesystem::remove(target_path);
                        }
                        write_message(client_fd, resp.to_json().dump());
                    }
                    continue;
                }
                else if (req.cmd == "DOWNLOAD") {
                    std::string remote_path = req.args.value("remote_path", "");
                    uint64_t start_offset = req.args.value("start_offset", 0ULL);
                    std::string target_path = normalize_path(current_dir, remote_path, user_root);

                    spdlog::info("DOWNLOAD request: remote_path={}, current_dir={}, target_path={}, start_offset={}", remote_path, current_dir, target_path, start_offset);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    } else if (!std::filesystem::exists(target_path)) {
                        spdlog::warn("DOWNLOAD: File does not exist: {}", target_path);
                        resp = Response::error(203, "File not found");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    } else if (!std::filesystem::is_regular_file(target_path)) {
                        spdlog::warn("DOWNLOAD: Path is not a regular file: {}", target_path);
                        resp = Response::error(203, "Path is not a file");
                        write_message(client_fd, resp.to_json().dump());
                        continue;
                    } else {
                        uint64_t file_size = std::filesystem::file_size(target_path);
                        std::string file_hash = hash_file(target_path);
                        resp = Response::ok({{"file_size", file_size}, {"file_hash", file_hash}});
                        std::string resp_json_str = resp.to_json().dump();
                        spdlog::info("DOWNLOAD response: {}", resp_json_str);
                        write_message(client_fd, resp_json_str);

                        std::ifstream file(target_path, std::ios::binary);
                        if (file) {
                            if (start_offset > 0) {
                                file.seekg(static_cast<std::streamoff>(start_offset));
                            }
                            char buffer[65536];
                            uint64_t remaining = file_size - start_offset;
                            while (remaining > 0 && file.read(buffer, static_cast<std::streamsize>(std::min(remaining, static_cast<uint64_t>(sizeof(buffer)))))) {
                                size_t to_send = static_cast<size_t>(file.gcount());
                                ssize_t sent = send(client_fd, buffer, to_send, 0);
                                if (sent <= 0) {
                                    spdlog::error("Failed to send binary data");
                                    break;
                                }
                                remaining -= static_cast<uint64_t>(sent);
                            }
                            spdlog::info("DOWNLOAD: sent {} bytes", file_size - start_offset - remaining);
                        } else {
                            spdlog::error("Failed to open file for download: {}", target_path);
                        }
                        continue;
                    }
                }
                else if (req.cmd == "SYNC_LIST") {
                    std::string path = req.args.value("path", "");
                    std::string target_path = path.empty() ? current_dir : normalize_path(current_dir, path, user_root);

                    if (!is_path_safe(target_path, user_root)) {
                        resp = Response::error(200, "Path traversal not allowed");
                    } else {
                        nlohmann::json files = nlohmann::json::array();
                        if (std::filesystem::exists(target_path)) {
                            for (const auto& entry : std::filesystem::recursive_directory_iterator(target_path)) {
                                if (entry.is_regular_file()) {
                                    std::string rel_path = std::filesystem::relative(entry.path(), user_root).string();
                                    std::string hash = hash_file(entry.path().string());
                                    nlohmann::json f;
                                    f["path"] = rel_path;
                                    f["hash"] = hash;
                                    files.push_back(f);
                                }
                            }
                        }
                        resp = Response::ok({{"files", files}});
                    }
                }
                else {
                    spdlog::warn("Unknown command: {}", req.cmd);
                    resp = Response::error(300, "Unknown command");
                }

                if (req.cmd != "DOWNLOAD" && req.cmd != "UPLOAD" && req.cmd != "RESUME_UPLOAD") {
                    spdlog::info("Sending response for command {}: {}", req.cmd, resp.to_json().dump());
                    write_message(client_fd, resp.to_json().dump());
                }
            } catch (const std::exception& e) {
                Response resp = Response::error(301, std::string("Error: ") + e.what());
                write_message(client_fd, resp.to_json().dump());
            }
        }
    } catch (...) {
    }

    if (!username.empty()) {
        spdlog::info("User {} disconnected", username);
    } else {
        spdlog::info("Client fd={} disconnected", client_fd);
    }
    ::close(client_fd);
}

}
