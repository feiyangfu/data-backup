// 写入 src/crypto.h
#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

const char MAGIC_STRING[] = "CRPTV1";
const int SALT_SIZE = 16; // 盐的大小
const int HASH_SIZE = 32; // SHA256 哈希的大小

// 定义在加密文件头部的元数据结构
struct CryptoHeader {
    char magic[8];         // "CRYPT V1" 魔术字符串，用于识别文件类型
    unsigned char salt[SALT_SIZE];     // 随机生成的盐
    unsigned char password_hash[HASH_SIZE]; // 用于验证密码的哈希
};

// 加密函数
bool encrypt_file(const fs::path& input_file, const fs::path& output_file, const std::string& password);

// 解密函数
bool decrypt_file(const fs::path& input_file, const fs::path& output_file, const std::string& password);

#endif // CRYPTO_H