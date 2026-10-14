#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

// FNV-1a hash with 64-bit output: https://gist.github.com/branw/66cd5eddb07786957f9b48fe0a85b7c7
size_t FNV1a(const uint8_t *data, size_t dataLen) {
    uint64_t hash = 14695981039346656037ull;
    for (size_t i = 0; i < dataLen; i++) {
        hash ^= data[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

uint64_t compute_file_hash(const fs::path& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    return FNV1a(buffer.data(), buffer.size());
}

// Convert file time to date string (YYYY-MM-DD) - Internet
std::string get_date_string(const fs::file_time_type& file_time) {
    auto sys_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        file_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    std::time_t tt = std::chrono::system_clock::to_time_t(sys_time);
    std::tm* tm = std::localtime(&tt);

    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}


struct SummaryOutput {
    size_t files_scanned = 0;
    size_t duplicates_found = 0;
    size_t duplicates_removed = 0;
    size_t photos_organized = 0;
    std::vector<std::pair<std::string, std::string>> errors;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Wrong number of arguments: " << argv[0] << " <input_directory> <output_directory> [--keep-duplicates] [--ext <extension>]" << std::endl;
        return 1;
    }

    std::string input_dir = argv[1];
    std::string output_dir = argv[2];
    bool keep_duplicates = false;
    std::string filter_extension = "";

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--keep-duplicates") {
            keep_duplicates = true;
        } else if (arg == "--ext" && i + 1 < argc) {
            filter_extension = argv[++i];
            std::transform(filter_extension.begin(), filter_extension.end(), filter_extension.begin(), ::tolower);
        }
    }

    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        std::cerr << "Invalid input directory path: " << input_dir << std::endl;
        return 1;
    }

    try {
        fs::create_directories(output_dir);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Unable to create output directory: " << e.what() << std::endl;
        return 1;
    }

    fs::path duplicates_dir;
    if (keep_duplicates) {
        duplicates_dir = fs::path(output_dir) / "_duplicates";
        try {
            fs::create_directories(duplicates_dir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Unable to create duplicates directory: " << e.what() << std::endl;
            return 1;
        }
    }

    SummaryOutput summary_output;

    std::unordered_map<uintmax_t, std::vector<fs::path>> files_by_size;

    const std::unordered_set<std::string> image_extensions = {
        "jpg", "jpeg", "png", "bmp", "gif", "tiff", "tif", "webp"
    };

    try {
        for (const auto& file : fs::recursive_directory_iterator(input_dir)) {
            if (!file.is_regular_file()) {
                continue;
            }

            try {
                std::string ext = file.path().extension().string();
                if (ext.empty()) {
                    continue;
                }

                ext = ext.substr(1);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (!filter_extension.empty()) {
                    if (ext != filter_extension) {
                        continue;
                    }
                } else {
                    if (image_extensions.count(ext) == 0) {
                        continue;
                    }
                }

                summary_output.files_scanned++;
                uintmax_t file_size = fs::file_size(file.path());
                files_by_size[file_size].push_back(file.path());

            } catch (const fs::filesystem_error& e) {
                summary_output.errors.push_back({file.path().string(), e.what()});
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Unable to scan input directory: " << e.what() << std::endl;
        return 1;
    }

    std::unordered_set<fs::path> duplicate_files;
    std::vector<fs::path> unique_files;

    for (const auto& [size, paths] : files_by_size) {
        if (paths.size() < 2) {
            for (const auto& path : paths) {
                unique_files.push_back(path);
            }
            continue;
        }

        std::unordered_map<uint64_t, std::vector<fs::path>> hash_groups;

        for (const auto& path : paths) {
            try {
                uint64_t hash = compute_file_hash(path);
                if (hash != 0) {
                    hash_groups[hash].push_back(path);
                } else {
                    summary_output.errors.push_back({path.string(), "Cannot read file for hashing"});
                }
            } catch (const std::exception& e) {
                summary_output.errors.push_back({path.string(), std::string("Hash error: ") + e.what()});
            }
        }

        for (const auto& [hash, hash_paths] : hash_groups) {
            if (hash_paths.size() > 1) {
                summary_output.duplicates_found += hash_paths.size() - 1;
                unique_files.push_back(hash_paths[0]);
                for (size_t i = 1; i < hash_paths.size(); i++) {
                    duplicate_files.insert(hash_paths[i]);
                }
            } else {
                unique_files.push_back(hash_paths[0]);
            }
        }
    }

    for (const auto& dup_path : duplicate_files) {
        try {
            if (keep_duplicates) {
                std::string stem = dup_path.stem().string();
                std::string ext = dup_path.extension().string();
                std::string base_name = stem;
                fs::path target = duplicates_dir / (base_name + ext);

                int suffix = 0;
                while (fs::exists(target)) {
                    suffix++;
                    target = duplicates_dir / (base_name + "_" + std::to_string(suffix) + ext);
                }
                fs::rename(dup_path, target);
            } else {
                fs::remove(dup_path);
            }
            summary_output.duplicates_removed++;
        } catch (const fs::filesystem_error& e) {
            summary_output.errors.push_back({dup_path.string(), std::string("Duplicate handling error: ") + e.what()});
        }
    }

    for (const auto& file_path : unique_files) {
        try {
            auto file_time = fs::last_write_time(file_path);
            std::string date_str = get_date_string(file_time);

            std::string year = date_str.substr(0, 4);
            std::string month = date_str.substr(5, 2);

            fs::path target_dir = fs::path(output_dir) / year / month;
            fs::create_directories(target_dir);

            std::string ext = file_path.extension().string();
            std::string stem = file_path.stem().string();

            std::string base_name = date_str + "_" + stem;
            fs::path target_path = target_dir / (base_name + ext);

            int suffix = 0;
            while (fs::exists(target_path)) {
                suffix++;
                target_path = target_dir / (base_name + "_" + std::to_string(suffix) + ext);
            }

            fs::rename(file_path, target_path);
            summary_output.photos_organized++;

        } catch (const std::exception& e) {
            summary_output.errors.push_back({file_path.string(), std::string("Error: ") + e.what()});
        }
    }

    std::cout << "Files scanned: " << summary_output.files_scanned << std::endl;
    std::cout << "Duplicates found: " << summary_output.duplicates_found << std::endl;
    std::cout << (keep_duplicates ? "Duplicates moved: " : "Duplicates removed: ") << summary_output.duplicates_removed << std::endl;
    std::cout << "Photos organized: " << summary_output.photos_organized << std::endl;
    std::cout << "Errors encountered: " << summary_output.errors.size() << std::endl;

    if (!summary_output.errors.empty()) {
        for (const auto& [file, error] : summary_output.errors) {
            std::cout << "- " << file << ": " << error << std::endl;
        }
    }

    return 0;
}
