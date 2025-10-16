#include "backup.h"
#include "archive.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>

namespace fs = std::filesystem;

std::map<ino_t, std::string> hard_link_map;

// 手动递归遍历和打包函数 (简化版)
void traverseAndPack(const fs::path& base_path, const fs::path& current_path, std::ofstream& archive_file) {
    DIR *dir = opendir(current_path.c_str());
    if (!dir) {
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        fs::path full_path = current_path / entry->d_name;
        
        ArchiveHeader header = {};
        struct stat file_stat;

        if (lstat(full_path.c_str(), &file_stat) != 0) {
            continue;
        }

        // --- 简化点 #1：如果遇到软链接，直接跳过，不处理 ---
        if (S_ISLNK(file_stat.st_mode)) {
            std::cout << "Skipping symlink: " << full_path.string() << std::endl;
            continue;
        }

        fs::path relative_path = fs::relative(full_path, base_path);
        strncpy(header.name, relative_path.c_str(), sizeof(header.name) - 1);
        
        header.mode = file_stat.st_mode;
        header.uid = file_stat.st_uid;
        header.gid = file_stat.st_gid;
        header.mtime = file_stat.st_mtime;
        header.size = 0;

        if (S_ISREG(header.mode)) {
            header.size = file_stat.st_size;
            if (file_stat.st_nlink > 1) {
                if (hard_link_map.count(file_stat.st_ino)) {
                    strncpy(header.link_target, hard_link_map[file_stat.st_ino].c_str(), sizeof(header.link_target) - 1);
                    header.size = 0; 
                } else {
                    hard_link_map[file_stat.st_ino] = relative_path.string();
                }
            }
        } 
        
        archive_file.write(reinterpret_cast<const char*>(&header), sizeof(ArchiveHeader));
        std::cout << "Packed: " << relative_path.string() << std::endl;

        if (S_ISREG(header.mode) && header.size > 0) {
            std::ifstream source_file(full_path, std::ios::binary);
            char buffer[4096];
            while (source_file.good()) {
                source_file.read(buffer, sizeof(buffer));
                archive_file.write(buffer, source_file.gcount());
            }
            
            size_t padding_size = (BLOCK_SIZE - (header.size % BLOCK_SIZE)) % BLOCK_SIZE;
            for (size_t i = 0; i < padding_size; ++i) {
                archive_file.put('\0');
            }
        }
        
        if (S_ISDIR(header.mode)) {
            traverseAndPack(base_path, full_path, archive_file);
        }
    }
    closedir(dir);
}

// pack 入口函数 (简化版)
void pack(const fs::path& source_dir, const fs::path& archive_file_path) {
    hard_link_map.clear();
    std::ofstream archive_file(archive_file_path, std::ios::binary);
    if (!archive_file) return;

    ArchiveHeader root_header = {};
    struct stat root_stat;
    if (lstat(source_dir.c_str(), &root_stat) == 0) {
        strncpy(root_header.name, ".", sizeof(root_header.name) - 1);
        root_header.mode = root_stat.st_mode;
        // ... (其他元数据)
        archive_file.write(reinterpret_cast<const char*>(&root_header), sizeof(ArchiveHeader));
        std::cout << "Packed: ." << std::endl;
    }
    traverseAndPack(source_dir, source_dir, archive_file);
    archive_file.close();
}

// unpack 函数 (简化版)
void unpack(const fs::path& archive_file_path, const fs::path& destination_dir) {
    std::ifstream archive_file(archive_file_path, std::ios::binary);
    if (!archive_file) return;

    if (!fs::exists(destination_dir)) {
        fs::create_directories(destination_dir);
    }

    ArchiveHeader header;
    while (archive_file.read(reinterpret_cast<char*>(&header), sizeof(ArchiveHeader))) {
        fs::path full_path = destination_dir / header.name;
        
        // --- 简化点 #2：移除所有 S_ISLNK 的处理逻辑 ---
        if (S_ISDIR(header.mode)) {
            fs::create_directories(full_path);
        } 
        else if (S_ISREG(header.mode)) {
            if (header.size == 0 && strlen(header.link_target) > 0) {
                fs::path original_file_path = destination_dir / header.link_target;
                link(original_file_path.c_str(), full_path.c_str());
            } else {
                std::ofstream dest_file(full_path, std::ios::binary);
                uint64_t bytes_to_read = header.size;
                char buffer[4096];
                while (bytes_to_read > 0) {
                    std::streamsize read_size = std::min((uint64_t)sizeof(buffer), bytes_to_read);
                    archive_file.read(buffer, read_size);
                    dest_file.write(buffer, archive_file.gcount());
                    bytes_to_read -= archive_file.gcount();
                }
                
                size_t padding_size = (BLOCK_SIZE - (header.size % BLOCK_SIZE)) % BLOCK_SIZE;
                archive_file.seekg(padding_size, std::ios::cur);
            }
        }
        
        // 恢复元数据
        if (strcmp(header.name, ".") != 0) {
             chmod(full_path.c_str(), header.mode);
             // ... (其他元数据恢复)
        }
        std::cout << "Unpacked: " << full_path.string() << std::endl;
    }
}