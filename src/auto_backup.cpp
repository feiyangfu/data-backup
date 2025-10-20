#include "backup.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

// 函数声明
void run_auto_backup(const AutoBackupOptions& options);

void run_auto_backup(const AutoBackupOptions& options) {
    std::cout << "=== Starting Automatic Backup Task ===" << std::endl;

    // --- 步骤 1: 数据淘汰 ---
    std::cout << "--> Step 1: Cleaning up old backups..." << std::endl;
    std::vector<fs::path> existing_backups;

    // 扫描备份目录，找出所有符合命名规则的旧备份
    for (const auto& entry : fs::directory_iterator(options.destination_dir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // 检查文件名是否以 base_name 开头，并以 .dat 结尾
            if (filename.rfind(options.base_name, 0) == 0 && filename.substr(filename.length() - 4) == ".dat") {
                existing_backups.push_back(entry.path());
            }
        }
    }

    // 按文件名（也就是时间戳）排序，旧的在前，新的在后
    std::sort(existing_backups.begin(), existing_backups.end());

    // 计算需要删除的备份数量
    if (existing_backups.size() >= options.keep_latest) {
        int to_delete_count = existing_backups.size() - options.keep_latest + 1;
        std::cout << "Found " << existing_backups.size() << " backups. Keeping " << options.keep_latest << ". Deleting " << to_delete_count << " old backup(s)." << std::endl;
        
        for (int i = 0; i < to_delete_count; ++i) {
            std::cout << "Deleting: " << existing_backups[i] << std::endl;
            fs::remove(existing_backups[i]);
        }
    } else {
        std::cout << "Found " << existing_backups.size() << " backups. No old backups to delete." << std::endl;
    }

    // --- 步骤 2: 执行新备份 ---
    std::cout << "\n--> Step 2: Performing new backup..." << std::endl;

    // a. 生成带有时间戳的新备份文件名
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y%m%d-%H%M%S", ltm);

    std::string new_backup_filename = options.base_name + "-" + time_str + ".dat";
    fs::path new_backup_filepath = options.destination_dir / new_backup_filename;

    std::cout << "Source: " << options.source_dir << std::endl;
    std::cout << "Destination: " << new_backup_filepath << std::endl;

    // b. 调用我们现有的 pack 函数
    pack(options.source_dir, new_backup_filepath, options.filters, options.password, options.use_compression);

    std::cout << "\n=== Automatic Backup Task Finished ===" << std::endl;
}