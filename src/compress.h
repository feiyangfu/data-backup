// 写入 src/compress.h
#ifndef COMPRESS_H
#define COMPRESS_H

#include <filesystem>

namespace fs = std::filesystem;

// 压缩文件
bool compress_file(const fs::path& input_file, const fs::path& output_file);

// 解压文件
bool decompress_file(const fs::path& input_file, const fs::path& output_file);

#endif // COMPRESS_H