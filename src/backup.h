#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>
#include <string>
#include <vector>     
#include <cstdint>   
#include <ctime>     

namespace fs = std::filesystem;

struct FilterOptions {
    // 按文件名/路径排除 (支持简单通配符 *)
    std::vector<std::string> include_patterns;
    std::vector<std::string> exclude_patterns;

    // 按文件类型排除 (使用 mode_t 类型)
    std::vector<mode_t> exclude_types;

    // 按文件大小筛选
    uint64_t min_size = 0;              
    uint64_t max_size = UINT64_MAX;    

    // 按修改时间筛选
    time_t older_than = 0;              // 只备份比这个时间戳更老的文件
    time_t newer_than = 0;              // 只备份比这个时间戳更新的文件
};

struct AutoBackupOptions {
    fs::path source_dir;             // 要备份的源目录
    fs::path destination_dir;        // 存放所有备份文件的目录
    int keep_latest = 5;             // 保留的最新备份数量
    std::string base_name = "backup"; // 备份文件名的基础部分
    FilterOptions filters;           // 应用的过滤规则
    std::string password = "";       // 加密密码
    bool use_compression = false;    // 是否压缩
};

void run_auto_backup(const AutoBackupOptions& options);

void pack(const fs::path& source_dir, const fs::path& archive_file_path, const FilterOptions& filters, std::string_view password = "", bool use_compression = false);

void unpack(const fs::path& archive_file_path, const fs::path& destination_dir, std::string_view password = "", bool use_compression = false);
#endif 