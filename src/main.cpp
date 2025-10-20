#include "backup.h" // 包含所有公开函数的声明
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

// 更新用法说明，加入 auto-backup 命令
void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "  " << prog_name << " pack <source_dir> <archive_file> [OPTIONS...]" << std::endl;
    std::cerr << "  " << prog_name << " unpack <archive_file> <dest_dir> [OPTIONS...]" << std::endl;
    std::cerr << "  " << prog_name << " auto-backup [AUTO_OPTIONS...]" << std::endl;
    
    std::cerr << "\nPACK/UNPACK Options:" << std::endl;
    std::cerr << "  --compress                Enable compression." << std::endl;
    std::cerr << "  --include <pattern>       Only include files/dirs matching pattern." << std::endl;
    std::cerr << "  --exclude <pattern>       Exclude files/dirs matching pattern (e.g., '*.log')." << std::endl;
    std::cerr << "  --min-size <bytes>        Only include files larger than this size." << std::endl;
    std::cerr << "  <password>                Set encryption password." << std::endl;
    
    std::cerr << "\nAUTO-BACKUP Options (format: --key=value):" << std::endl;
    std::cerr << "  --source=<path>           Required. Source directory to backup." << std::endl;
    std::cerr << "  --dest=<path>             Required. Destination directory for backups." << std::endl;
    std::cerr << "  --keep=<number>           Number of latest backups to keep (default: 5)." << std::endl;
    std::cerr << "  --name=<base_name>        Base name for backup files (default: 'backup')." << std::endl;
    std::cerr << "  --password=<pass>         Encryption password." << std::endl;
    std::cerr << "  --compress=<true|false>   Enable compression." << std::endl;
    std::cerr << "  --include=<pattern>       Include filter pattern." << std::endl;
    std::cerr << "  --exclude=<pattern>       Exclude filter pattern." << std::endl;
}

// main 函数现在是整个程序的总调度器
int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;

    if (command == "pack" || command == "unpack") {
        if (argc < 4) {
            print_usage(argv[0]);
            return 1;
        }
        fs::path source = argv[2];
        fs::path destination = argv[3];
        
        bool use_compression = false;
        std::string password = "";
        FilterOptions filters;

        std::vector<std::string> args(argv + 4, argv + argc);
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--compress") {
                use_compression = true;
            } else if (args[i] == "--include") {
                if (i + 1 < args.size()) filters.include_patterns.push_back(args[++i]);
            } else if (args[i] == "--exclude") {
                if (i + 1 < args.size()) filters.exclude_patterns.push_back(args[++i]);
            } else if (args[i] == "--min-size") {
                if (i + 1 < args.size()) filters.min_size = std::stoull(args[++i]);
            } else {
                if (password.empty()) password = args[i];
            }
        }
        
        if (command == "pack") {
            pack(source, destination, filters, password, use_compression);
            success = true; // 假设 pack 内部会处理失败情况
        } else {
            unpack(source, destination, password, use_compression);
            success = true; // 假设 unpack 内部会处理失败情况
        }

    } else if (command == "auto-backup") {
        AutoBackupOptions auto_opts;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.rfind("--", 0) != 0) continue; // 只处理 -- 开头的参数
            
            size_t pos = arg.find('=');
            if (pos == std::string::npos) continue;

            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);

            if (key == "source") auto_opts.source_dir = value;
            else if (key == "dest") auto_opts.destination_dir = value;
            else if (key == "keep") auto_opts.keep_latest = std::stoi(value);
            else if (key == "name") auto_opts.base_name = value;
            else if (key == "password") auto_opts.password = value;
            else if (key == "compress") auto_opts.use_compression = (value == "true");
            else if (key == "include") auto_opts.filters.include_patterns.push_back(value);
            else if (key == "exclude") auto_opts.filters.exclude_patterns.push_back(value);
        }

        if (auto_opts.source_dir.empty() || auto_opts.destination_dir.empty()) {
            std::cerr << "Error: --source and --dest are required for auto-backup." << std::endl;
            print_usage(argv[0]);
            return 1;
        }

        run_auto_backup(auto_opts);
        success = true; // 假设 run_auto_backup 内部会处理失败

    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Operation finished in " << duration.count() << " seconds." << std::endl;

    return success ? 0 : 1;
}