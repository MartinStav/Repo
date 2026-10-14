#pragma once

#include <string>

namespace minidrive {

std::string hash_password(const std::string& password, const std::string& salt);
bool verify_password(const std::string& password, const std::string& hash, const std::string& salt);
std::string hash_file(const std::string& path);

}
