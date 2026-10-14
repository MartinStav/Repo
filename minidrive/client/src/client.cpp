#include "client.hpp"
#include "minidrive/protocol.hpp"
#include "minidrive/crypto.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <map>
#include <vector>

namespace minidrive {

namespace {
    Client* g_client = nullptr;

    void signal_handler(int sig) {
        if (g_client && sig == SIGINT) {
            g_client->disconnect();
            exit(0);
        }
    }
}

Client::Client(const std::string& host, std::uint16_t port, const std::string& username, const std::string& log_file)
    : host_(host), port_(port), username_(username), log_file_(log_file), fd_(-1), connected_(false) {
    g_client = this;
    signal(SIGINT, signal_handler);
}

Client::~Client() {
    disconnect();
}

bool Client::connect() {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        std::perror("socket");
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    if (::inet_pton(AF_INET, host_.c_str(), &addr.sin_addr) != 1) {
        std::cerr << "Invalid IPv4 address: " << host_ << std::endl;
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    if (::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("connect");
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    connected_ = true;

    if (username_.empty()) {
        std::cout << "[warning] operating in public mode - files are visible to everyone" << std::endl;
        nlohmann::json auth = {{"type", "public"}};
        write_message(fd_, auth.dump());

        std::string resp_msg = read_message(fd_);
        if (resp_msg.empty()) {
            std::cerr << "Connection lost" << std::endl;
            disconnect();
            return false;
        }
    } else {
        std::cout << "Password: ";
        std::string password;
        std::getline(std::cin, password);

        nlohmann::json auth = {{"type", "auth"}, {"username", username_}, {"password", password}};
        write_message(fd_, auth.dump());

        std::string resp_msg = read_message(fd_);
        if (resp_msg.empty()) {
            std::cerr << "Connection lost" << std::endl;
            disconnect();
            return false;
        }

        auto resp_json = nlohmann::json::parse(resp_msg);
        if (resp_json["status"] == "ERROR") {
            int code = resp_json["code"];
            std::string msg = resp_json["message"];

            if (code == 102) {
                std::cout << "User " << username_ << " not found. Register? (y/n): ";
                std::string answer;
                std::getline(std::cin, answer);
                if (answer == "y" || answer == "Y") {
                    std::cout << "Password: ";
                    std::string new_password;
                    std::getline(std::cin, new_password);

                    nlohmann::json reg_req = {{"type", "register"}, {"username", username_}, {"password", new_password}};
                    write_message(fd_, reg_req.dump());

                    std::string reg_resp = read_message(fd_);
                    if (reg_resp.empty()) {
                        std::cerr << "No response from server" << std::endl;
                    } else {
                        try {
                            auto reg_json = nlohmann::json::parse(reg_resp);
                            if (reg_json["status"] == "OK") {
                                std::cout << "User registered successfully" << std::endl;
                            } else {
                                std::cerr << "Registration failed: " << reg_json.value("message", "Unknown error") << std::endl;
                            }
                        } catch (const std::exception& e) {
                            std::cerr << "Error parsing response: " << e.what() << std::endl;
                            std::cerr << "Response was: " << reg_resp << std::endl;
                        }
                    }
                }
            } else {
                std::cerr << "Authentication failed: " << msg << std::endl;
            }
            disconnect();
            return false;
        } else {
            std::string message = resp_json.value("data", nlohmann::json::object()).value("message", "");
            if (!message.empty()) {
                std::cout << message << std::endl;
            }
        }
    }

    return true;
}

void Client::disconnect() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    connected_ = false;
}

void Client::run() {
    if (!log_file_.empty()) {
        try {
            auto file_logger = spdlog::basic_logger_mt("client", log_file_);
            spdlog::set_default_logger(file_logger);
            spdlog::info("Client started");
        } catch (...) {
        }
    }

    if (!connect()) {
        return;
    }

    std::string current_dir = ".";
    std::vector<std::string> resume_commands;

    if (!username_.empty()) {
        struct IncompleteTransfer {
            std::string type;
            std::string local_path;
            std::string remote_path;
            std::string part_file;
        };

        std::vector<IncompleteTransfer> incomplete_transfers;

        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().filename().string();

                if (name.find("_download_") == 0) {
                    std::string local_path = name.substr(10);
                    IncompleteTransfer t;
                    t.type = "download";
                    t.local_path = local_path;
                    t.remote_path = local_path;
                    t.part_file = name;
                    incomplete_transfers.push_back(t);
                }
                else if (name.find("_upload_") == 0) {
                    IncompleteTransfer t;
                    t.type = "upload";
                    t.part_file = name;
                    std::ifstream marker(name);
                    if (marker.is_open()) {
                        std::getline(marker, t.local_path);
                        std::getline(marker, t.remote_path);
                        marker.close();
                    }
                    incomplete_transfers.push_back(t);
                }
            }
        }

        if (!incomplete_transfers.empty()) {
            std::cout << "Incomplete upload/downloads detected, resume? (y/n): ";
            std::string answer;
            std::getline(std::cin, answer);
            if (answer == "y" || answer == "Y") {
                for (const auto& transfer : incomplete_transfers) {
                    if (transfer.type == "upload") {
                        std::string local_path = transfer.local_path;
                        std::string remote_path = transfer.remote_path;

                        if (local_path.empty() || remote_path.empty()) {
                            std::ifstream marker(transfer.part_file);
                            if (marker.is_open()) {
                                std::getline(marker, local_path);
                                std::getline(marker, remote_path);
                                marker.close();
                            }
                        }

                        if (!local_path.empty() && !remote_path.empty() &&
                            std::filesystem::exists(local_path) && std::filesystem::is_regular_file(local_path)) {
                            std::cout << "> UPLOAD " << local_path << " " << remote_path << std::endl;
                            std::filesystem::remove(transfer.part_file);
                            resume_commands.push_back("UPLOAD " + local_path + " " + remote_path);
                        } else {
                            std::cout << "Cannot resume upload: local file not found or invalid marker" << std::endl;
                            std::filesystem::remove(transfer.part_file);
                        }
                    } else if (transfer.type == "download") {
                        std::string remote_path = transfer.remote_path;
                        std::string local_path = transfer.local_path;

                        if (remote_path.empty()) {
                            remote_path = local_path;
                        }

                        std::cout << "> DOWNLOAD " << remote_path << " " << local_path << std::endl;
                        resume_commands.push_back("DOWNLOAD " + remote_path + " " + local_path);
                    }
                }
            } else {
                for (const auto& transfer : incomplete_transfers) {
                    std::filesystem::remove(transfer.part_file);
                }
            }
        }
    }

    if (!username_.empty()) {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().filename().string();
                if (name.find("_upload_") == 0) {
                    std::ifstream marker(name);
                    if (marker.is_open()) {
                        std::string local_path, remote_path;
                        std::getline(marker, local_path);
                        std::getline(marker, remote_path);
                        marker.close();
                        if (!local_path.empty() && !remote_path.empty() &&
                            std::filesystem::exists(local_path) && std::filesystem::is_regular_file(local_path)) {
                            resume_commands.push_back("UPLOAD " + local_path + " " + remote_path);
                        }
                    }
                }
            }
        }
    }

    while (connected_) {
        std::string line;

        if (!resume_commands.empty()) {
            line = resume_commands.front();
            resume_commands.erase(resume_commands.begin());
            std::cout << "> " << line << std::endl;
        } else {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) {
                break;
            }
        }

        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (cmd == "HELP") {
            spdlog::debug("CMD: HELP");
            std::cout << "Available commands:" << std::endl;
            std::cout << "  HELP - Show this help" << std::endl;
            std::cout << "  EXIT - Exit client" << std::endl;
            std::cout << "  LIST [path] - List files and directories" << std::endl;
            std::cout << "  CD <path> - Change directory" << std::endl;
            std::cout << "  MKDIR <path> - Create directory" << std::endl;
            std::cout << "  RMDIR <path> - Remove directory" << std::endl;
            std::cout << "  DELETE <path> - Delete file" << std::endl;
            std::cout << "  MOVE <src> <dst> - Move/rename file or directory" << std::endl;
            std::cout << "  COPY <src> <dst> - Copy file or directory" << std::endl;
            std::cout << "  UPLOAD <local_path> [remote_path] - Upload file" << std::endl;
            std::cout << "  DOWNLOAD <remote_path> [local_path] - Download file" << std::endl;
            std::cout << "  SYNC <src> <dst> - Synchronize directory" << std::endl;
            continue;
        }

        if (cmd == "EXIT") {
            spdlog::info("CMD: EXIT");
            break;
        }

        if (!connected_) break;

        try {
            if (cmd == "LIST") {
                std::string path;
                iss >> path;

                nlohmann::json req = {{"cmd", "LIST"}, {"args", {{"path", path}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                    if (resp_json.contains("data") && resp_json["data"].contains("entries")) {
                        auto entries = resp_json["data"]["entries"];
                        for (const auto& entry : entries) {
                            std::string type = entry["type"];
                            std::string name = entry["name"];
                            if (type == "dir") {
                                std::cout << "  [DIR]  " << name << std::endl;
                            } else {
                                uint64_t size = entry.value("size", 0ULL);
                                std::cout << "  [FILE] " << name << " (" << size << " bytes)" << std::endl;
                            }
                        }
                    }
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "CD") {
                std::string path;
                iss >> path;
                if (path.empty()) {
                    std::cerr << "Usage: CD <path>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "CD"}, {"args", {{"path", path}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "MKDIR") {
                std::string path;
                iss >> path;
                if (path.empty()) {
                    std::cerr << "Usage: MKDIR <path>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "MKDIR"}, {"args", {{"path", path}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "RMDIR") {
                std::string path;
                iss >> path;
                if (path.empty()) {
                    std::cerr << "Usage: RMDIR <path>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "RMDIR"}, {"args", {{"path", path}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "DELETE") {
                std::string path;
                iss >> path;
                if (path.empty()) {
                    std::cerr << "Usage: DELETE <path>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "DELETE"}, {"args", {{"path", path}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "MOVE") {
                std::string src, dst;
                iss >> src >> dst;
                if (src.empty() || dst.empty()) {
                    std::cerr << "Usage: MOVE <src> <dst>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "MOVE"}, {"args", {{"src", src}, {"dst", dst}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "COPY") {
                std::string src, dst;
                iss >> src >> dst;
                if (src.empty() || dst.empty()) {
                    std::cerr << "Usage: COPY <src> <dst>" << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "COPY"}, {"args", {{"src", src}, {"dst", dst}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "UPLOAD") {
                std::string local_path, remote_path;
                iss >> local_path;
                if (local_path.empty()) {
                    std::cerr << "Usage: UPLOAD <local_path> [remote_path]" << std::endl;
                    continue;
                }
                iss >> remote_path;
                if (remote_path.empty()) {
                    remote_path = std::filesystem::path(local_path).filename().string();
                }

                std::string remote_hash = std::to_string(std::hash<std::string>{}(remote_path));
                std::string upload_marker = "_upload_" + remote_hash;

                if (!std::filesystem::exists(local_path) || !std::filesystem::is_regular_file(local_path)) {
                    std::cerr << "Local file not found: " << local_path << std::endl;
                    continue;
                }

                if (std::filesystem::exists(remote_path)) {
                    std::cerr << "Remote file already exists: " << remote_path << std::endl;
                    continue;
                }

                uint64_t file_size = std::filesystem::file_size(local_path);
                std::string file_hash = hash_file(local_path);
                uint64_t start_offset = 0;

                if (!username_.empty()) {
                    nlohmann::json check_req = {{"cmd", "RESUME_UPLOAD_CHECK"}, {"args", {{"remote_path", remote_path}}}};
                    write_message(fd_, check_req.dump());

                    std::string check_resp = read_message(fd_);
                    if (!check_resp.empty()) {
                        auto check_json = nlohmann::json::parse(check_resp);
                        if (check_json["status"] == "OK") {
                            start_offset = check_json["data"].value("uploaded_bytes", 0ULL);
                        }
                    }
                }

                if (start_offset > 0) {
                    nlohmann::json req = {{"cmd", "RESUME_UPLOAD"}, {"args", {{"remote_path", remote_path}, {"file_size", file_size}, {"start_offset", start_offset}, {"file_hash", file_hash}}}};
                    write_message(fd_, req.dump());
                } else {
                    nlohmann::json req = {{"cmd", "UPLOAD"}, {"args", {{"remote_path", remote_path}, {"file_size", file_size}, {"file_hash", file_hash}}}};
                    write_message(fd_, req.dump());
                }

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] != "OK") {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                    continue;
                }

                std::ifstream file(local_path, std::ios::binary);
                if (!file) {
                    std::cerr << "Cannot open local file" << std::endl;
                    continue;
                }

                if (start_offset > 0) {
                    file.seekg(static_cast<std::streamoff>(start_offset));
                }

                char buffer[65536];
                uint64_t remaining = file_size - start_offset;
                while (remaining > 0 && file.read(buffer, static_cast<std::streamsize>(std::min(remaining, static_cast<uint64_t>(sizeof(buffer)))))) {
                    size_t to_send = static_cast<size_t>(file.gcount());
                    send(fd_, buffer, to_send, 0);
                    remaining -= to_send;
                }

                resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    if (!username_.empty()) {
                        std::ofstream marker(upload_marker);
                        marker << local_path << "\n" << remote_path << "\n";
                    }
                    break;
                }

                resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] == "OK") {
                    if (!username_.empty() && std::filesystem::exists(upload_marker)) {
                        std::filesystem::remove(upload_marker);
                    }
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                }
            }
            else if (cmd == "DOWNLOAD") {
                std::string remote_path, local_path;
                iss >> remote_path;
                if (remote_path.empty()) {
                    std::cerr << "Usage: DOWNLOAD <remote_path> [local_path]" << std::endl;
                    continue;
                }
                iss >> local_path;
                if (local_path.empty()) {
                    local_path = std::filesystem::path(remote_path).filename().string();
                }

                uint64_t start_offset = 0;
                std::string part_path = "_download_" + local_path;
                if (!username_.empty() && std::filesystem::exists(part_path)) {
                    start_offset = std::filesystem::file_size(part_path);
                    local_path = part_path;
                } else if (std::filesystem::exists(local_path) && std::filesystem::is_regular_file(local_path)) {
                    std::cerr << "Local file already exists: " << local_path << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "DOWNLOAD"}, {"args", {{"remote_path", remote_path}, {"start_offset", start_offset}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                nlohmann::json resp_json;
                try {
                    resp_json = nlohmann::json::parse(resp_msg);
                } catch (const std::exception& e) {
                    std::cerr << "Failed to parse response: " << e.what() << std::endl;
                    std::cerr << "Response was: " << resp_msg.substr(0, 200) << std::endl;
                    continue;
                }

                if (resp_json["status"] != "OK") {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                    continue;
                }

                if (!resp_json.contains("data")) {
                    std::cerr << "Invalid response from server - missing data field" << std::endl;
                    continue;
                }
                if (!resp_json["data"].contains("file_size") || !resp_json["data"].contains("file_hash")) {
                    std::cerr << "Invalid response from server - missing file_size or file_hash. Response: " << resp_msg << std::endl;
                    continue;
                }

                uint64_t file_size = resp_json["data"]["file_size"];
                std::string file_hash = resp_json["data"]["file_hash"];

                std::ios_base::openmode mode = (start_offset == 0) ? std::ios::binary : (std::ios::binary | std::ios::app);
                std::ofstream file(local_path, mode);
                if (!file) {
                    std::cerr << "Cannot create local file" << std::endl;
                    continue;
                }

                if (start_offset > 0) {
                    file.seekp(static_cast<std::streamoff>(start_offset));
                }

                uint64_t remaining = file_size - start_offset;
                char buffer[65536];
                while (remaining > 0) {
                    size_t to_read = std::min(remaining, static_cast<uint64_t>(sizeof(buffer)));
                    ssize_t n = recv(fd_, buffer, to_read, 0);
                    if (n <= 0) {
                        std::cerr << "Connection lost during download" << std::endl;
                        file.close();
                        if (!username_.empty()) {
                            std::string part_name = "_download_" + std::filesystem::path(remote_path).filename().string();
                            std::filesystem::rename(local_path, part_name);
                        } else {
                            std::filesystem::remove(local_path);
                        }
                        break;
                    }
                    file.write(buffer, n);
                    remaining -= static_cast<uint64_t>(n);
                }
                file.close();

                if (remaining == 0) {
                    std::string final_path = local_path;
                    if (!username_.empty() && local_path.find("_download_") == 0) {
                        final_path = local_path.substr(10);
                        std::filesystem::rename(local_path, final_path);
                    }

                    std::string computed_hash = hash_file(final_path);
                    if (computed_hash == file_hash) {
                        std::cout << "OK" << std::endl;
                    } else {
                        std::cout << "ERROR: Hash mismatch" << std::endl;
                        std::filesystem::remove(final_path);
                    }
                }
            }
            else if (cmd == "SYNC") {
                std::string src, dst;
                iss >> src >> dst;
                if (src.empty() || dst.empty()) {
                    std::cerr << "Usage: SYNC <src> <dst>" << std::endl;
                    continue;
                }

                if (!std::filesystem::exists(src) || !std::filesystem::is_directory(src)) {
                    std::cerr << "Local directory not found: " << src << std::endl;
                    continue;
                }

                nlohmann::json req = {{"cmd", "SYNC_LIST"}, {"args", {{"path", dst}}}};
                write_message(fd_, req.dump());

                std::string resp_msg = read_message(fd_);
                if (resp_msg.empty()) {
                    std::cerr << "Connection lost" << std::endl;
                    break;
                }

                auto resp_json = nlohmann::json::parse(resp_msg);
                if (resp_json["status"] != "OK") {
                    std::cout << "ERROR: " << resp_json["code"] << std::endl;
                    std::cout << resp_json["message"] << std::endl;
                    continue;
                }

                std::map<std::string, std::string> remote_files;
                std::filesystem::path dst_path(dst);
                for (const auto& f : resp_json["data"]["files"]) {
                    std::string full_path = f["path"];
                    std::filesystem::path fp(full_path);
                    std::filesystem::path rel = fp.lexically_relative(dst_path);
                    if (rel.string().find("..") == std::string::npos) {
                        remote_files[rel.string()] = f["hash"];
                    }
                }

                std::map<std::string, std::string> local_files;
                for (const auto& entry : std::filesystem::recursive_directory_iterator(src)) {
                    if (entry.is_regular_file()) {
                        std::string rel_path = std::filesystem::relative(entry.path(), src).string();
                        std::string fhash = hash_file(entry.path().string());
                        local_files[rel_path] = fhash;
                    }
                }

                int uploaded = 0, deleted = 0, skipped = 0;

                for (const auto& [fpath, fhash] : local_files) {
                    auto it = remote_files.find(fpath);
                    if (it == remote_files.end() || it->second != fhash) {
                        std::string local_file = (std::filesystem::path(src) / fpath).string();
                        std::string remote_file = (dst_path / fpath).string();

                        std::filesystem::path remote_dir = std::filesystem::path(remote_file).parent_path();
                        if (!remote_dir.empty() && remote_dir != dst_path) {
                            nlohmann::json mkdir_req = {{"cmd", "MKDIR"}, {"args", {{"path", remote_dir.string()}}}};
                            write_message(fd_, mkdir_req.dump());
                            read_message(fd_);
                        }

                        if (it != remote_files.end()) {
                            nlohmann::json del_req = {{"cmd", "DELETE"}, {"args", {{"path", remote_file}}}};
                            write_message(fd_, del_req.dump());
                            read_message(fd_);
                        }

                        std::cout << "Uploading: " << fpath << std::endl;

                        uint64_t fsize = std::filesystem::file_size(local_file);
                        nlohmann::json upload_req = {{"cmd", "UPLOAD"}, {"args", {{"remote_path", remote_file}, {"file_size", fsize}, {"file_hash", fhash}}}};
                        write_message(fd_, upload_req.dump());

                        resp_msg = read_message(fd_);
                        if (resp_msg.empty()) {
                            std::cerr << "Connection lost" << std::endl;
                            break;
                        }

                        resp_json = nlohmann::json::parse(resp_msg);
                        if (resp_json["status"] == "OK") {
                            std::ifstream file(local_file, std::ios::binary);
                            if (file) {
                                char buf[65536];
                                uint64_t rem = fsize;
                                while (rem > 0) {
                                    size_t chunk = std::min(rem, static_cast<uint64_t>(sizeof(buf)));
                                    file.read(buf, static_cast<std::streamsize>(chunk));
                                    auto got = static_cast<size_t>(file.gcount());
                                    if (got == 0) break;
                                    send(fd_, buf, got, 0);
                                    rem -= got;
                                }

                                resp_msg = read_message(fd_);
                                if (resp_msg.empty()) {
                                    std::cerr << "Connection lost" << std::endl;
                                    break;
                                }

                                resp_json = nlohmann::json::parse(resp_msg);
                                if (resp_json["status"] == "OK") {
                                    uploaded++;
                                }
                            }
                        }
                    } else {
                        skipped++;
                    }
                }

                for (const auto& [fpath, fhash] : remote_files) {
                    if (local_files.find(fpath) == local_files.end()) {
                        std::string remote_file = (dst_path / fpath).string();

                        std::cout << "Deleting: " << fpath << std::endl;

                        nlohmann::json del_req = {{"cmd", "DELETE"}, {"args", {{"path", remote_file}}}};
                        write_message(fd_, del_req.dump());

                        resp_msg = read_message(fd_);
                        if (resp_msg.empty()) {
                            std::cerr << "Connection lost" << std::endl;
                            break;
                        }

                        resp_json = nlohmann::json::parse(resp_msg);
                        if (resp_json["status"] == "OK") {
                            deleted++;
                        }
                    }
                }

                std::cout << "OK" << std::endl;
                std::cout << "Uploaded: " << uploaded << ", Deleted: " << deleted << ", Skipped: " << skipped << std::endl;
            }
            else {
                std::cerr << "Unknown command: " << cmd << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    disconnect();
}

}
