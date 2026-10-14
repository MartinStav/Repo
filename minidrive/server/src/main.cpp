#include <iostream>
#include <string>
#include <cstdint>
#include <sodium.h>
#include <spdlog/spdlog.h>

#include "minidrive/version.hpp"
#include "server.hpp"

int main(int argc, char* argv[]) {
    std::uint16_t port = 9000;
    std::string root_path = "./data/server_root";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = static_cast<std::uint16_t>(std::stoi(argv[++i]));
        } else if (arg == "--root" && i + 1 < argc) {
            root_path = argv[++i];
        }
    }

    if (sodium_init() < 0) {
        spdlog::error("Failed to initialize libsodium");
        return 1;
    }

    spdlog::info("MiniDrive server v{} starting on port {}", minidrive::version(), port);
    minidrive::Server server(root_path, port);
    server.start();
    spdlog::info("Server stopped");
    return 0;
}
