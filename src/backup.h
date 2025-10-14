#ifndef BACKUP_H
#define BACKUP_H

#include <filesystem>

namespace fs = std::filesystem;

void copyFileContent(const fs::path& from, const fs::path& to);
void processDirectory(const fs::path& source, const fs::path& destination);

#endif // BACKUP_H