#pragma once

#include <string>
#include <map>

namespace minidrive {

class UserDB {
public:
    UserDB(const std::string& db_path);

    bool user_exists(const std::string& username) const;
    bool register_user(const std::string& username, const std::string& password);
    bool authenticate(const std::string& username, const std::string& password) const;

private:
    std::string db_path_;
    mutable std::map<std::string, std::pair<std::string, std::string>> users_;

    void load();
    void save() const;
};

}
