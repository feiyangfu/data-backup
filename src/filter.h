#ifndef FILTER_H
#define FILTER_H

#include "backup.h" 
#include <filesystem>
#include <sys/stat.h>

namespace fs = std::filesystem;

class Filter {
public:

    explicit Filter(const FilterOptions& options);

    bool should_skip(const fs::path& file_path, const struct stat& file_stat) const;

private:
    const FilterOptions& opts;

    bool wildcard_match(const std::string& text, const std::string& pattern) const;
};

#endif // FILTER_H