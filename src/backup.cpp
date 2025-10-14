#include "backup.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

// 使用 C++17 的 filesystem 库
namespace fs = std::filesystem;

/**
 * @brief 复制单个文件的内容
 * @param from 源文件路径
 * @param to 目标文件路径
 */
void copyFileContent(const fs::path& from, const fs::path& to) {
    std::ifstream in(from, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Cannot open source file: " << from << std::endl;
        return;
    }

    std::ofstream out(to, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot create destination file: " << to << std::endl;
        return;
    }
    
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer))) {
        out.write(buffer, in.gcount());
    }
    out.write(buffer, in.gcount());
}

/**
 * @brief 备份或还原整个目录树
 * @param source 源目录
 * @param destination 目标目录
 */
void processDirectory(const fs::path& source, const fs::path& destination) {
    if (!fs::exists(source) || !fs::is_directory(source)) {
        std::cerr << "Error: Source path is not a valid directory: " << source << std::endl;
        return;
    }
    
    if (!fs::exists(destination)) {
        fs::create_directories(destination);
    }

    for (const auto& entry : fs::recursive_directory_iterator(source)) {
        fs::path relative_path = fs::relative(entry.path(), source);
        fs::path destination_path = destination / relative_path;

        if (fs::is_directory(entry.path())) {
            if (!fs::exists(destination_path)) {
                fs::create_directory(destination_path);
            }
        } else if (fs::is_regular_file(entry.path())) {
            copyFileContent(entry.path(), destination_path);
        }
    }
    std::cout << "Process completed successfully!" << std::endl;
}
