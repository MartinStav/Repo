#include "minidrive/user_db.hpp"
#include "minidrive/crypto.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace minidrive {

UserDB::UserDB(const std::string& db_path) : db_path_(db_path) {
    load();
}

void UserDB::load() {
    if (!std::filesystem::exists(db_path_)) {
        return;
    }

    std::ifstream file(db_path_);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t first_space = line.find(' ');
        if (first_space == std::string::npos) continue;

        std::string username = line.substr(0, first_space);
        std::string hash = line.substr(first_space + 1);

        users_[username] = {hash, ""};
    }
}

void UserDB::save() const {
    std::filesystem::create_directories(std::filesystem::path(db_path_).parent_path());
    std::ofstream file(db_path_, std::ios::trunc);
    for (const auto& [username, creds] : users_) {
        file << username << " " << creds.first << "\n";
    }
    file.flush();
    file.close();
}

bool UserDB::user_exists(const std::string& username) const {
    return users_.find(username) != users_.end();
}

bool UserDB::register_user(const std::string& username, const std::string& password) {
    if (user_exists(username)) {
        return false;
    }

    std::string hash = hash_password(password, "");
    if (hash.empty()) {
        return false;
    }

    users_[username] = {hash, ""};
    save();
    std::filesystem::path db_dir = std::filesystem::path(db_path_).parent_path();
    std::filesystem::create_directories(db_dir / "users" / username);
    return true;
}

bool UserDB::authenticate(const std::string& username, const std::string& password) const {
    auto it = users_.find(username);
    if (it == users_.end()) {
        return false;
    }

    return verify_password(password, it->second.first, it->second.second);
}

}
