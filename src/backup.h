#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>

namespace fs = std::filesystem;

// 只存放函数的声明
void pack(const fs::path& source_dir, const fs::path& archive_file_path);
void unpack(const fs::path& archive_file_path, const fs::path& destination_dir);

#endif // BACKUP_H