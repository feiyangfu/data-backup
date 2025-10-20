#include "filter.h"
#include <iostream>

Filter::Filter(const FilterOptions& options) : opts(options) {}

// 简单的通配符匹配函数 (只支持 *)
// (这个函数保持不变)
bool Filter::wildcard_match(const std::string& text, const std::string& pattern) const {
    if (pattern.empty()) return text.empty();
    if (pattern == "*") return true;

    auto star_pos = pattern.find('*');
    if (star_pos == std::string::npos) {
        return text == pattern;
    }

    if (text.rfind(pattern.substr(0, star_pos), 0) != 0) {
        return false;
    }
    if (text.length() < pattern.length() - 1) {
        return false;
    }
    return text.find(pattern.substr(star_pos + 1), text.length() - (pattern.length() - star_pos - 1)) != std::string::npos;
}


// --- 这是修改后的 should_skip 函数 ---
bool Filter::should_skip(const fs::path& file_path, const struct stat& file_stat) const {
    const std::string filename = file_path.filename().string();
    const std::string fullpath = file_path.string();

    // --- 新增：Include 规则判断 ---
    // 检查是否设置了任何 include 规则
    if (!opts.include_patterns.empty()) {
        bool is_included = false;
        // 遍历所有的 include 模式
        for (const auto& pattern : opts.include_patterns) {
            // 只要文件名或完整路径匹配任意一个 include 模式，就认为它应该被包含
            if (wildcard_match(filename, pattern) || wildcard_match(fullpath, pattern)) {
                is_included = true;
                break; // 找到一个匹配项就足够了，跳出循环
            }
        }
        // 如果遍历完所有 include 模式后，该文件仍然没有被标记为包含，那么就跳过它
        if (!is_included) {
            std::cout << "Filter: Skipping '" << fullpath << "' (does not match any --include pattern)" << std::endl;
            return true;
        }
    }
    // --- 结束新增部分 ---
    // 注意：只有当一个文件通过了上面的 include 检查（或者根本没有设置 include 规则时），代码才会继续执行到这里


    // 规则1：按文件名/路径模式排除 (这部分代码保持不变)
    for (const auto& pattern : opts.exclude_patterns) {
        if (wildcard_match(filename, pattern) || wildcard_match(fullpath, pattern)) {
            std::cout << "Filter: Skipping '" << fullpath << "' due to --exclude pattern '" << pattern << "'" << std::endl;
            return true;
        }
        if ( ("/" + fullpath + "/").find("/" + pattern + "/") != std::string::npos ) {
            std::cout << "Filter: Skipping '" << fullpath << "' because its path contains excluded directory '" << pattern << "'" << std::endl;
            return true;
    }
    }
    
    // 规则2：按文件类型排除 (这部分代码保持不变)
    for (const auto& type : opts.exclude_types) {
        if ((file_stat.st_mode & S_IFMT) == type) {
            std::cout << "Filter: Skipping '" << fullpath << "' due to type exclusion." << std::endl;
            return true;
        }
    }

    // 只对普通文件进行尺寸和时间过滤 (这部分代码保持不变)
    if (S_ISREG(file_stat.st_mode)) {
        if (file_stat.st_size < opts.min_size) {
            std::cout << "Filter: Skipping '" << fullpath << "' (size " << file_stat.st_size << " < " << opts.min_size << ")" << std::endl;
            return true;
        }

        if (file_stat.st_size > opts.max_size) {
            std::cout << "Filter: Skipping '" << fullpath << "' (size " << file_stat.st_size << " > " << opts.max_size << ")" << std::endl;
            return true;
        }
        
        if (opts.older_than > 0 && file_stat.st_mtime >= opts.older_than) {
            std::cout << "Filter: Skipping '" << fullpath << "' (not older than specified timestamp)" << std::endl;
            return true;
        }

        if (opts.newer_than > 0 && file_stat.st_mtime <= opts.newer_than) {
            std::cout << "Filter: Skipping '" << fullpath << "' (not newer than specified timestamp)" << std::endl;
            return true;
        }
    }

    // 所有规则都通过，不跳过
    return false;
}