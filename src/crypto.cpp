#include "crypto.h"
#include <iostream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <fstream>
#include <cstring>

const int KEY_SIZE = 32;
const int IV_SIZE = 16;

void handle_openssl_errors() {
    ERR_print_errors_fp(stderr);
}

bool generate_key_iv(const std::string& password, const unsigned char* salt, unsigned char* key, unsigned char* iv) {
    unsigned char combined_key_iv[KEY_SIZE + IV_SIZE];
    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                           salt, SALT_SIZE,
                           10000,
                           EVP_sha256(),
                           KEY_SIZE + IV_SIZE,
                           combined_key_iv)) {
        handle_openssl_errors();
        return false;
    }
    memcpy(key, combined_key_iv, KEY_SIZE);
    memcpy(iv, combined_key_iv + KEY_SIZE, IV_SIZE);
    return true;
}

bool encrypt_file(const fs::path& input_file, const fs::path& output_file, const std::string& password) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Cannot open input file for encryption: " << input_file << std::endl;
        return false;
    }

    std::ofstream ofs(output_file, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Cannot create output file for encryption: " << output_file << std::endl;
        return false;
    }

    CryptoHeader header;
    strncpy(header.magic, MAGIC_STRING, sizeof(header.magic));
    header.magic[sizeof(header.magic) - 1] = '\0';
    
    if (!RAND_bytes(header.salt, SALT_SIZE)) {
        handle_openssl_errors();
        return false;
    }

    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];
    if (!generate_key_iv(password, header.salt, key, iv)) return false;

    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), header.salt, SALT_SIZE, 20000, EVP_sha256(), HASH_SIZE, header.password_hash)) {
         handle_openssl_errors();
         return false;
    }
    
    ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
    

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) { handle_openssl_errors(); return false; }
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handle_openssl_errors();
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    const int buffer_size = 4096;
    std::vector<char> buffer(buffer_size);
    std::vector<unsigned char> ciphertext(buffer_size + EVP_MAX_BLOCK_LENGTH);
    int len;

    while (ifs.good()) {
        ifs.read(buffer.data(), buffer_size);
        if (ifs.gcount() > 0) {
            if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(buffer.data()), ifs.gcount())) {
                handle_openssl_errors();
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            ofs.write(reinterpret_cast<const char*>(ciphertext.data()), len);
        }
    }

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data(), &len)) {
        handle_openssl_errors();
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ofs.write(reinterpret_cast<const char*>(ciphertext.data()), len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_file(const fs::path& input_file, const fs::path& output_file, const std::string& password) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) { 
        std::cerr << "Error: Cannot open input file for decryption: " << input_file << std::endl;
        return false; 
    }
    
    CryptoHeader header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (!ifs) {
        std::cerr << "Error: Failed to read crypto header from file: " << input_file << std::endl;
        return false;
    }

    if (strncmp(header.magic, MAGIC_STRING, 8) != 0) {
        std::cerr << "Error: Not a valid encrypted file or unsupported version." << std::endl;
        return false;
    }

    unsigned char verify_hash[HASH_SIZE];
    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), header.salt, SALT_SIZE, 20000, EVP_sha256(), HASH_SIZE, verify_hash)) {
        handle_openssl_errors();
        return false;
    }

    if (memcmp(header.password_hash, verify_hash, HASH_SIZE) != 0) {
        std::cerr << "Error: Invalid password." << std::endl;
        return false;
    }

    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];
    if (!generate_key_iv(password, header.salt, key, iv)) return false;

    std::ofstream ofs(output_file, std::ios::binary);
    if (!ofs) { 
        std::cerr << "Error: Cannot create temporary file for decryption: " << output_file << std::endl;
        return false; 
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handle_openssl_errors();
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    const int buffer_size = 4096;
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<char> plaintext(buffer_size + EVP_MAX_BLOCK_LENGTH);
    int len;

    while (ifs.good()) {
        ifs.read(reinterpret_cast<char*>(buffer.data()), buffer_size);
        if (ifs.gcount() > 0) {
            if (1 != EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len, buffer.data(), ifs.gcount())) {
                handle_openssl_errors();
                EVP_CIPHER_CTX_free(ctx);
                return false;
            }
            ofs.write(plaintext.data(), len);
        }
    }

    if (1 != EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len)) {
        handle_openssl_errors();
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ofs.write(plaintext.data(), len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}