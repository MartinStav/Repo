#include "minidrive/crypto.hpp"

#include <sodium.h>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace minidrive {

std::string hash_password(const std::string& password, const std::string& /* salt */) {
    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(hash, password.c_str(), password.length(),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        return "";
    }

    return std::string(hash);
}

bool verify_password(const std::string& password, const std::string& hash, const std::string& /* salt */) {
    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}

std::string hash_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return "";
    }

    unsigned char hash[crypto_generichash_BYTES];
    crypto_generichash_state state;
    crypto_generichash_init(&state, nullptr, 0, sizeof(hash));

    char buffer[65536];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        crypto_generichash_update(&state, reinterpret_cast<unsigned char*>(buffer), static_cast<unsigned long long>(file.gcount()));
    }

    crypto_generichash_final(&state, hash, sizeof(hash));

    std::stringstream ss;
    for (size_t i = 0; i < sizeof(hash); ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

}
