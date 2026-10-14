#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <csignal>
#include <atomic>

namespace fs = std::filesystem;

struct Item {
    int sell_price;
    int store_price;
};

long long* g_resource = nullptr;
std::map<int, int>* g_inventory = nullptr;
fs::path* g_inventory_out_path = nullptr;
std::atomic<bool> g_should_exit(false);

void signal_handler(int signal) {
    if (g_inventory != nullptr && g_inventory_out_path != nullptr) {
        std::ofstream out_file(*g_inventory_out_path);
        if (out_file.is_open()) {
            for (const auto& [item_id, quantity] : *g_inventory) {
                if (quantity > 0) {
                    out_file << item_id << " " << quantity << "\n";
                }
            }
            out_file.close();
        }
    }

    if (g_resource != nullptr) {
        std::cout << *g_resource << std::endl;
    }

    g_should_exit = true;
    std::exit(0);
}

int main(int argc, char *argv[]) {

    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " inventory.exe <items_path> <inventory_path> <inventory_out_path> <resource>" << std::endl;
        return 1;
    }

    fs::path items_path = fs::path(argv[1]);
    fs::path inventory_path = fs::path(argv[2]);
    fs::path inventory_out_path = fs::path(argv[3]);
    long long resource = std::stoll(argv[4]);

    g_resource = &resource;
    g_inventory_out_path = &inventory_out_path;
    std::signal(SIGINT, signal_handler);

    if ((!fs::exists(items_path) || !fs::is_regular_file(items_path)) || (!fs::exists(inventory_path) || !fs::is_regular_file(inventory_path))) {
        std::cout << "Items or inventory path does not exist: " << items_path << " or " << inventory_path << std::endl;
        return 1;
    }

    if (!fs::exists(inventory_out_path)) {
        std::ofstream out_file(inventory_out_path);
        if (!out_file) {
            std::cout << "Failed to create output file: " << inventory_out_path << std::endl;
            return 1;
        }
        out_file.close();
    }

    std::map<int, Item> items;
    std::ifstream items_file(items_path);
    if (!items_file.is_open()) {
        std::cout << "Failed to open items file: " << items_path << std::endl;
        return 1;
    }
    std::string line;
    while (getline(items_file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        int item_id, sell_price, store_price;
        if (!(iss >> item_id >> sell_price >> store_price)) {
            std::cout << "Error: invalid data in items file" << std::endl;
            return 1;
        }
        if (sell_price <= 0 || store_price <= 0) {
            std::cout << "Error: invalid data in items file" << std::endl;
            return 1;
        }
        items[item_id] = {sell_price, store_price};
    }
    items_file.close();

    std::map<int, int> inventory;
    g_inventory = &inventory;
    std::ifstream inventory_file(inventory_path);
    if (!inventory_file.is_open()) {
        std::cout << "Failed to open inventory file: " << inventory_path << std::endl;
        return 1;
    }
    while (getline(inventory_file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        int item_id, quantity;
        if (!(iss >> item_id >> quantity) || quantity <= 0) {
            std::cout << "Error: invalid data in inventory file" << std::endl;
            return 1;
        }
        if (quantity <= 0) {
            std::cout << "Error: invalid data in inventory file" << std::endl;
            return 1;
        }
        if (items.find(item_id) == items.end()) {
            std::cout << "Error: non-existent item id in inventory file" << std::endl;
            return 1;
        }
        inventory[item_id] = quantity;
    }
    inventory_file.close();

    while (getline(std::cin, line) && !g_should_exit) {
        if (line.empty()) {
            break;
        }

        std::istringstream iss(line);
        std::string operation;
        iss >> operation;

        std::map<int, int> inventory_before = inventory;

        std::map<int, int> items_sold_today;
        std::map<int, int> items_stored_today;

        if (operation == "sell") {
            int item_id, quantity;
            if (!(iss >> item_id >> quantity) || quantity <= 0) {
                std::cout << "Error: invalid operation" << std::endl;
                return 1;
            }

            if (items.find(item_id) == items.end()) {
                std::cout << "Error: non-existent item id" << std::endl;
                return 1;
            }

            int available = (inventory.find(item_id) != inventory.end()) ? inventory[item_id] : 0;
            int sold = (quantity > available) ? available : quantity;

            if (sold > 0) {
                inventory[item_id] -= sold;
                if (inventory[item_id] == 0) {
                    inventory.erase(item_id);
                }
                resource += sold * items[item_id].sell_price;
                items_sold_today[item_id] = sold;
            }

        } else if (operation == "store") {
            int item_id, quantity;
            if (!(iss >> item_id >> quantity) || quantity <= 0) {
                std::cout << "Error: invalid operation" << std::endl;
                return 1;
            }

            if (items.find(item_id) == items.end()) {
                std::cout << "Error: non-existent item id" << std::endl;
                return 1;
            }

            inventory[item_id] += quantity;
            items_stored_today[item_id] = quantity;

        } else if (operation == "none") {
        } else {
            std::cout << "Error: invalid operation" << std::endl;
            return 1;
        }

        for (const auto& [item_id, quantity_before] : inventory_before) {
            int sold_today = items_sold_today.count(item_id) ? items_sold_today[item_id] : 0;

            int quantity_to_pay_for = quantity_before - sold_today;

            if (quantity_to_pay_for > 0) {
                resource -= quantity_to_pay_for * items[item_id].store_price;
            }
        }
    }

    std::ofstream out_file(inventory_out_path);
    if (!out_file.is_open()) {
        std::cout << "Error: cannot open output file" << std::endl;
        return 1;
    }

    for (const auto& [item_id, quantity] : inventory) {
        if (quantity > 0) {
            out_file << item_id << " " << quantity << "\n";
        }
    }
    out_file.close();

    std::cout << resource << std::endl;

    return 0;
}
