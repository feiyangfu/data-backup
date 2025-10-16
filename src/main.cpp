#include "backup.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "  " << prog_name << " pack <source_dir> <archive_file> [password]" << std::endl;
    std::cerr << "  " << prog_name << " unpack <archive_file> <dest_dir> [password]" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5) {
        print_usage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    fs::path source = argv[2];
    fs::path destination = argv[3];
    std::string password = (argc == 5) ? argv[4] : ""; // 如果有第5个参数，就把它当作密码

    auto start = std::chrono::high_resolution_clock::now();

    if (command == "pack") {
        pack(source, destination, password);
    } else if (command == "unpack") {
        unpack(source, destination, password);
    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Operation finished in " << duration.count() << " seconds." << std::endl;

    return 0;
}