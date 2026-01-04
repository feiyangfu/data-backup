#include "backup.h"
#include "archive.h"
#include "crypto.h"
#include "compress.h" 
#include "filter.h" 
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
#include <errno.h>

namespace fs = std::filesystem;

std::map<ino_t, std::string> hard_link_map;

// 前向声明
bool pack_internal(const fs::path& source_dir, const fs::path& archive_file_path, const FilterOptions& filters);
void unpack_internal(const fs::path& archive_file_path, const fs::path& destination_dir);
void traverseAndPack(const fs::path& base_path, const fs::path& current_path, std::ofstream& archive_file, const Filter& filter);

bool pack(const fs::path& source_dir, const fs::path& archive_file_path, const FilterOptions& filters, std::string_view password, bool use_compression) {
    fs::path current_input = source_dir;
    fs::path temp_pack_output;
    
    // 1. 打包
    std::cout << "--> Step 1: Packing files..." << std::endl;
    temp_pack_output = archive_file_path;
    temp_pack_output += ".pack.tmp";
    if (!pack_internal(current_input, temp_pack_output, filters)) {
        std::cerr << "--> Packing failed." << std::endl;
        fs::remove(temp_pack_output);
        return false;
    }
    current_input = temp_pack_output;

    // 2. (可选) 压缩
    fs::path temp_compress_output;
    if (use_compression) {
        std::cout << "--> Step 2: Compressing data..." << std::endl;
        temp_compress_output = archive_file_path;
        temp_compress_output += ".comp.tmp";
        if (!compress_file(current_input, temp_compress_output)) {
             std::cerr << "--> Compression failed." << std::endl;
             fs::remove(current_input);
             fs::remove(temp_compress_output);
             return false;
        }
        fs::remove(current_input); 
        current_input = temp_compress_output;
    }

    // 3. (可选) 加密
    if (!password.empty()) {
        std::cout << "--> Step 3: Encrypting data..." << std::endl;
        if (!encrypt_file(current_input, archive_file_path, std::string(password))) {
             std::cerr << "--> Encryption failed." << std::endl;
             fs::remove(current_input);
             return false;
        }
        fs::remove(current_input);
    } else {
        // 如果不加密，上一步的输出就是最终文件
        fs::rename(current_input, archive_file_path);
    }

    std::cout << "--> Pack operation completed successfully." << std::endl;
    return true;
}

bool unpack(const fs::path& archive_file_path, const fs::path& destination_dir, std::string_view password, bool use_compression) {
    fs::path current_input = archive_file_path;
    
    // 1. (可选) 解密
    fs::path temp_decrypt_output;
    if (!password.empty()) {
        std::cout << "--> Step 1: Decrypting data..." << std::endl;
        temp_decrypt_output = archive_file_path;
        temp_decrypt_output += ".decrypt.tmp";
        if(!decrypt_file(current_input, temp_decrypt_output, std::string(password))) {
            std::cerr << "--> Decryption failed." << std::endl;
            fs::remove(temp_decrypt_output);
            return false;
        }
        current_input = temp_decrypt_output;
    }

    // 2. (可选) 解压
    fs::path temp_decompress_output;
    if (use_compression) {
        std::cout << "--> Step 2: Decompressing data..." << std::endl;
        temp_decompress_output = archive_file_path;
        temp_decompress_output += ".decompress.tmp";
        if (!decompress_file(current_input, temp_decompress_output)) {
            std::cerr << "--> Decompression failed." << std::endl;
            if(!password.empty()) fs::remove(current_input); // 清理上一步的临时文件
            fs::remove(temp_decompress_output);
            return false;
        }
        if(!password.empty()) fs::remove(current_input);
        current_input = temp_decompress_output;
    }

    // 3. 解包
    std::cout << "--> Step 3: Unpacking files..." << std::endl;
    unpack_internal(current_input, destination_dir);

    // 清理最后的临时文件
    if (use_compression || !password.empty()) {
        fs::remove(current_input);
    }

    std::cout << "--> Unpack operation completed successfully." << std::endl;
    return true;
}


bool pack_internal(const fs::path& source_dir, const fs::path& archive_file_path, const FilterOptions& filters) {
    hard_link_map.clear();
    std::ofstream archive_file(archive_file_path, std::ios::binary);

    if (!archive_file) {
        std::cerr << "Error: Cannot create archive file: " << archive_file_path << std::endl;
        return false;
    }
    Filter filter(filters);
    ArchiveHeader root_header = {};
    struct stat root_stat;
    if (lstat(source_dir.c_str(), &root_stat) == 0) {
        strncpy(root_header.name, ".", sizeof(root_header.name) - 1);
        root_header.name[sizeof(root_header.name) - 1] = '\0';
        root_header.mode = root_stat.st_mode;
        root_header.uid = root_stat.st_uid;
        root_header.gid = root_stat.st_gid;
        root_header.mtime = root_stat.st_mtime;
        archive_file.write(reinterpret_cast<const char*>(&root_header), sizeof(ArchiveHeader));
        std::cout << "Packed: ." << std::endl;
    } else {
        std::cerr << "Error: Cannot stat root directory: " << source_dir << std::endl;
        return false;
    }
    
    traverseAndPack(source_dir, source_dir, archive_file, filter);
    archive_file.close();
    return true;
}

void traverseAndPack(const fs::path& base_path, const fs::path& current_path, std::ofstream& archive_file, const Filter& filter) {
    DIR *dir = opendir(current_path.c_str());
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        fs::path full_path = current_path / entry->d_name;
        
        ArchiveHeader header = {};
        struct stat file_stat;
        if (lstat(full_path.c_str(), &file_stat) != 0) continue;
        
        if (!S_ISDIR(file_stat.st_mode) && !S_ISLNK(file_stat.st_mode) && filter.should_skip(full_path, file_stat)) {
            continue; // 如果是文件且被过滤，则跳过（软链接不过滤，因为它们不占用空间）
        }

        // 计算相对路径（不解引用符号链接）
        fs::path relative_path;
        if (current_path == base_path) {
            relative_path = entry->d_name;
        } else {
            fs::path current_relative = fs::relative(current_path, base_path);
            relative_path = current_relative / entry->d_name;
        }
        
        strncpy(header.name, relative_path.c_str(), sizeof(header.name) - 1);
        header.name[sizeof(header.name) - 1] = '\0';
        
        header.mode = file_stat.st_mode;
        header.uid = file_stat.st_uid;
        header.gid = file_stat.st_gid;
        header.mtime = file_stat.st_mtime;
        header.size = 0;
        header.link_target[0] = '\0';

        // 处理软链接
        if (S_ISLNK(header.mode)) {
            char link_buffer[sizeof(header.link_target)];
            ssize_t len = readlink(full_path.c_str(), link_buffer, sizeof(link_buffer) - 1);
            if (len != -1) {
                link_buffer[len] = '\0';
                strncpy(header.link_target, link_buffer, sizeof(header.link_target) - 1);
                header.link_target[sizeof(header.link_target) - 1] = '\0';
                std::cout << "Packed symlink: " << relative_path.string() << " -> " << header.link_target << std::endl;
            } else {
                std::cerr << "Warning: Failed to read symlink: " << full_path << std::endl;
                closedir(dir);
                return;
            }
        }
        // 处理普通文件（包括硬链接）
        else if (S_ISREG(header.mode)) {
            header.size = file_stat.st_size;
            if (file_stat.st_nlink > 1) {
                if (hard_link_map.count(file_stat.st_ino)) {
                    strncpy(header.link_target, hard_link_map[file_stat.st_ino].c_str(), sizeof(header.link_target) - 1);
                    header.link_target[sizeof(header.link_target) - 1] = '\0';
                    header.size = 0; 
                } else {
                    hard_link_map[file_stat.st_ino] = relative_path.string();
                }
            }
        } 
        
        archive_file.write(reinterpret_cast<const char*>(&header), sizeof(ArchiveHeader));
        
        // 打印信息（软链接在上面已经打印了）
        if (!S_ISLNK(header.mode)) {
            std::cout << "Packed: " << relative_path.string() << std::endl;
        }

        if (S_ISREG(header.mode) && header.size > 0) {
            std::ifstream source_file(full_path, std::ios::binary);
            char buffer[4096];
            while (source_file.good()) {
                source_file.read(buffer, sizeof(buffer));
                if (source_file.gcount() > 0) {
                    archive_file.write(buffer, source_file.gcount());
                }
            }
            size_t padding_size = (BLOCK_SIZE - (header.size % BLOCK_SIZE)) % BLOCK_SIZE;
            for (size_t i = 0; i < padding_size; ++i) archive_file.put('\0');
        }
        
        if (S_ISDIR(header.mode)) {
            traverseAndPack(base_path, full_path, archive_file, filter);
        }
    }
    closedir(dir);
}

// void unpack_internal(const fs::path& archive_file_path, const fs::path& destination_dir) {
//     std::ifstream archive_file(archive_file_path, std::ios::binary);
//     if (!archive_file) {
//         std::cerr << "Error: Cannot open archive file for unpacking: " << archive_file_path << std::endl;
//         return;
//     }

//     if (!fs::exists(destination_dir)) fs::create_directories(destination_dir);

//     ArchiveHeader header;
//     while (archive_file.read(reinterpret_cast<char*>(&header), sizeof(ArchiveHeader))) {
//         fs::path full_path = destination_dir / header.name;
        
//         if (S_ISDIR(header.mode)) {
//             fs::create_directories(full_path);
//         } else if (S_ISREG(header.mode)) {
//             if (header.size == 0 && strlen(header.link_target) > 0) {
//                 fs::path original_file_path = destination_dir / header.link_target;
//                 link(original_file_path.c_str(), full_path.c_str());
//             } else {
//                 std::ofstream dest_file(full_path, std::ios::binary);
//                 uint64_t bytes_to_read = header.size;
//                 char buffer[4096];
//                 while (bytes_to_read > 0) {
//                     std::streamsize read_size = std::min((uint64_t)sizeof(buffer), bytes_to_read);
//                     archive_file.read(buffer, read_size);
//                     dest_file.write(buffer, archive_file.gcount());
//                     bytes_to_read -= archive_file.gcount();
//                 }
//                 size_t padding_size = (BLOCK_SIZE - (header.size % BLOCK_SIZE)) % BLOCK_SIZE;
//                 archive_file.seekg(padding_size, std::ios::cur);
//             }
//         }
        
//         chmod(full_path.c_str(), header.mode);
//         chown(full_path.c_str(), header.uid, header.gid);
//         struct utimbuf new_times = {header.mtime, header.mtime};
//         utime(full_path.c_str(), &new_times);
        
//         std::cout << "Unpacked: " << full_path.string() << std::endl;
//     }
// }

struct DeferredHardLink {
    fs::path link_source; // 原始文件路径
    fs::path link_dest;   // 要创建的新链接路径
};

void unpack_internal(const fs::path& archive_file_path, const fs::path& destination_dir) {
    std::ifstream archive_file(archive_file_path, std::ios::binary);
    if (!archive_file) {
        std::cerr << "Error: Cannot open archive file for unpacking: " << archive_file_path << std::endl;
        return;
    }

    if (!fs::exists(destination_dir)) fs::create_directories(destination_dir);

    // --- 新增：硬链接待办列表 ---
    std::vector<DeferredHardLink> deferred_links;

    ArchiveHeader header;
    while (archive_file.read(reinterpret_cast<char*>(&header), sizeof(ArchiveHeader))) {
        fs::path full_path = destination_dir / header.name;
        
        if (S_ISDIR(header.mode)) {
            fs::create_directories(full_path);
        } else if (S_ISLNK(header.mode)) {
            // 处理软链接
            if (strlen(header.link_target) > 0) {
                // 确保父目录存在
                if (full_path.has_parent_path()) {
                    fs::create_directories(full_path.parent_path());
                }
                // 如果文件已存在（使用 lstat 检查，不解引用软链接）
                struct stat st;
                if (lstat(full_path.c_str(), &st) == 0) {
                    // 文件存在，删除它
                    unlink(full_path.c_str());
                }
                // 创建软链接
                if (symlink(header.link_target, full_path.c_str()) == 0) {
                    std::cout << "Unpacked symlink: " << full_path.string() << " -> " << header.link_target << std::endl;
                } else {
                    std::cerr << "Warning: Failed to create symlink: " << full_path << " -> " << header.link_target 
                              << " (errno: " << errno << ")" << std::endl;
                }
            }
        } else if (S_ISREG(header.mode)) {
            // --- 修改硬链接处理逻辑 ---
            if (header.size == 0 && strlen(header.link_target) > 0) {
                // 不要立即创建，而是加入待办列表
                deferred_links.push_back({
                    destination_dir / header.link_target, // 源
                    full_path                             // 目标
                });
            } else { // 这是普通文件或第一个硬链接，正常解包

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

        // 元数据恢复逻辑
        if (S_ISLNK(header.mode)) {
            // 软链接：使用 lchown（不会解引用），软链接本身没有权限位
            lchown(full_path.c_str(), header.uid, header.gid);
            // 注意：大多数系统不支持修改软链接的时间戳，所以跳过 utime
        } else if (header.size != 0 || strlen(header.link_target) == 0) {
            // 普通文件和目录：正常恢复元数据
            chmod(full_path.c_str(), header.mode);
            chown(full_path.c_str(), header.uid, header.gid);
            struct utimbuf new_times = {header.mtime, header.mtime};
            utime(full_path.c_str(), &new_times);
        }
        
        // 打印信息（软链接在上面已经打印了）
        if (!S_ISLNK(header.mode)) {
            std::cout << "Unpacked: " << full_path.string() << std::endl;
        }
    }
    archive_file.close(); // 关闭文件流

    // ---处理所有待办的硬链接 ---
    std::cout << "--> Processing deferred hard links..." << std::endl;
    for (const auto& deferred : deferred_links) {
        if (fs::exists(deferred.link_source)) {
            link(deferred.link_source.c_str(), deferred.link_dest.c_str());
            std::cout << "Created hard link: " << deferred.link_dest << " -> " << deferred.link_source << std::endl;
        } else {
            std::cerr << "Warning: Original file for hard link '" << deferred.link_dest 
                      << "' not found at '" << deferred.link_source << "'. It might have been filtered during backup." << std::endl;
            // 创建一个空文件以作占位
            std::ofstream(deferred.link_dest).close();
        }
    }
}