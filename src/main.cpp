// src/main.cpp

#include "backup.h" 
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [backup|restore] <source_path> <destination_path>" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    fs::path source_path = argv[2];
    fs::path destination_path = argv[3];

    if (command == "backup" || command == "restore") {
        std::cout << "Starting " << command << "..." << std::endl;
        std::cout << "From: " << source_path << std::endl;
        std::cout << "To:   " << destination_path << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        processDirectory(source_path, destination_path);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Operation took " << duration.count() << " seconds." << std::endl;

    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        return 1;
    }

    return 0;
}