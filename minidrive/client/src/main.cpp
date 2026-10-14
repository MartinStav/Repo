#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <sodium.h>

#include "minidrive/version.hpp"
#include "client.hpp"

struct Endpoint {
    std::string username;
    std::string host;
    uint16_t port{};
};

static bool parse_endpoint(const std::string& input, Endpoint& out) {
    auto at_pos = input.find('@');
    std::string rest;
    if (at_pos != std::string::npos) {
        out.username = input.substr(0, at_pos);
        rest = input.substr(at_pos + 1);
    } else {
        rest = input;
    }

    auto colon = rest.rfind(':');
    if (colon == std::string::npos) return false;
    out.host = rest.substr(0, colon);
    std::string port_str = rest.substr(colon + 1);
    if (out.host.empty() || port_str.empty()) return false;
    char* end = nullptr;
    long p = std::strtol(port_str.c_str(), &end, 10);
    if (*end != '\0' || p < 0 || p > 65535) return false;
    out.port = static_cast<uint16_t>(p);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [username@]<host>:<port> [--log <log_file>]" << std::endl;
        return 1;
    }

    Endpoint ep;
    if (!parse_endpoint(argv[1], ep)) {
        std::cerr << "Invalid endpoint format: " << argv[1] << std::endl;
        return 1;
    }

    std::string log_file;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--log" && i + 1 < argc) {
            log_file = argv[++i];
        }
    }

    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium" << std::endl;
        return 1;
    }

    minidrive::Client client(ep.host, ep.port, ep.username, log_file);
    client.run();
    return 0;
}
