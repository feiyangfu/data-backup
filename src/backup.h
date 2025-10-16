#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>
#include <string> 
namespace fs = std::filesystem;

void pack(const fs::path& source_dir, const fs::path& archive_file_path, std::string_view password = "");

void unpack(const fs::path& archive_file_path, const fs::path& destination_dir, std::string_view password = "");
#endif 