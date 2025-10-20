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

    std::cout << "--> Step 1: Cleaning up old backups..." << std::endl;
    std::vector<fs::path> existing_backups;

    for (const auto& entry : fs::directory_iterator(options.destination_dir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // 检查文件名是否以 base_name 开头，并以 .dat 结尾
            if (filename.rfind(options.base_name, 0) == 0 && filename.substr(filename.length() - 4) == ".dat") {
                existing_backups.push_back(entry.path());
            }
        }
    }

    std::sort(existing_backups.begin(), existing_backups.end());

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

    std::cout << "\n--> Step 2: Performing new backup..." << std::endl;

    time_t now = time(0);
    tm *ltm = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y%m%d-%H%M%S", ltm);

    std::string new_backup_filename = options.base_name + "-" + time_str + ".dat";
    fs::path new_backup_filepath = options.destination_dir / new_backup_filename;

    std::cout << "Source: " << options.source_dir << std::endl;
    std::cout << "Destination: " << new_backup_filepath << std::endl;

    pack(options.source_dir, new_backup_filepath, options.filters, options.password, options.use_compression);

    std::cout << "\n=== Automatic Backup Task Finished ===" << std::endl;
}