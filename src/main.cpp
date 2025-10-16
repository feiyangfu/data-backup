#include "backup.h" // 包含 pack/unpack 声明
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [pack|unpack] <source> <destination>" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    fs::path source = argv[2];
    fs::path destination = argv[3];

    auto start = std::chrono::high_resolution_clock::now();

    if (command == "pack") {
        pack(source, destination);
    } else if (command == "unpack") {
        unpack(source, destination);
    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Operation finished in " << duration.count() << " seconds." << std::endl;

    return 0;
}