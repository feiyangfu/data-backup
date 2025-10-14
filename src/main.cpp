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
    // 以二进制模式打开输入文件
    std::ifstream in(from, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Cannot open source file: " << from << std::endl;
        return;
    }

    // 以二进制模式打开输出文件
    std::ofstream out(to, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot create destination file: " << to << std::endl;
        return;
    }
    
    // 使用缓冲区进行读写，提高效率
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer))) {
        out.write(buffer, in.gcount());
    }
    // 写入最后一部分不足缓冲区大小的数据
    out.write(buffer, in.gcount());
}

/**
 * @brief 备份或还原整个目录树
 * @param source 源目录
 * @param destination 目标目录
 */
void processDirectory(const fs::path& source, const fs::path& destination) {
    // 检查源路径是否存在且是一个目录
    if (!fs::exists(source) || !fs::is_directory(source)) {
        std::cerr << "Error: Source path is not a valid directory: " << source << std::endl;
        return;
    }
    
    // 如果目标路径不存在，则创建它
    if (!fs::exists(destination)) {
        fs::create_directories(destination);
    }

    // 使用 recursive_directory_iterator 递归遍历源目录
    for (const auto& entry : fs::recursive_directory_iterator(source)) {
        // 计算当前条目在目标目录中的对应路径
        fs::path relative_path = fs::relative(entry.path(), source);
        fs::path destination_path = destination / relative_path;

        if (fs::is_directory(entry.path())) {
            // 如果是目录，在目标位置创建对应的目录
            if (!fs::exists(destination_path)) {
                fs::create_directory(destination_path);
            }
        } else if (fs::is_regular_file(entry.path())) {
            // 如果是文件，复制其内容
            copyFileContent(entry.path(), destination_path);
        }
    }
    std::cout << "Process completed successfully!" << std::endl;
}

int main(int argc, char* argv[]) {
    // 简单的命令行参数解析
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
        processDirectory(source_path, destination_path);
    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        std::cerr << "Usage: " << argv[0] << " [backup|restore] <source_path> <destination_path>" << std::endl;
        return 1;
    }

    return 0;
}