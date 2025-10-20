#include <gtest/gtest.h>
#include "backup.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

namespace fs = std::filesystem;

// 辅助函数 compareDirectories 
bool compareDirectories(const fs::path& p1, const fs::path& p2) {
    auto it1 = fs::recursive_directory_iterator(p1, fs::directory_options::none);
    auto it2 = fs::recursive_directory_iterator(p2, fs::directory_options::none);

    while (it1 != fs::recursive_directory_iterator{} && it2 != fs::recursive_directory_iterator{}) {
        if (fs::relative(*it1, p1) != fs::relative(*it2, p2)) return false;
        if (it1->is_directory() != it2->is_directory()) return false;
        if (it1->is_symlink() != it2->is_symlink()) return false;
        
        if (it1->is_regular_file()) {
             if (it1->file_size() != it2->file_size()) return false;
        }
        
        ++it1;
        ++it2;
    }
    return it1 == fs::recursive_directory_iterator{} && it2 == fs::recursive_directory_iterator{};
}


class PackUnpackTest : public ::testing::Test {
protected:

    void SetUp() override {
        fs::create_directories(source_dir / "subdir");
        std::ofstream(source_dir / "file1.txt") << "hello";
        std::ofstream(source_dir / "subdir" / "file2.txt") << "world";

        link((source_dir / "file1.txt").c_str(), (source_dir / "hardlink.txt").c_str());
    }

    void TearDown() override {
        fs::remove_all(source_dir);
        fs::remove_all(unpack_dir);
        fs::remove(archive_file);
    }

    const fs::path source_dir = "test_source_dir";
    const fs::path unpack_dir = "test_unpack_dir";
    const fs::path archive_file = "test_archive.dat";

};


TEST_F(PackUnpackTest, FullCycleValidation) {
    // 创建一个空的过滤选项对象
    FilterOptions empty_filters;

    // 执行打包时，传入这个空的过滤选项
    pack(source_dir, archive_file, empty_filters); // <-- 传入第3个参数
    ASSERT_TRUE(fs::exists(archive_file));

    // 执行解包 (unpack 函数的签名我们没有改，所以这里不需要变)
    unpack(archive_file, unpack_dir);
    ASSERT_TRUE(fs::exists(unpack_dir));

    // 验证目录一致性
    ASSERT_TRUE(compareDirectories(source_dir, unpack_dir));
    
    // 验证硬链接 inode
    struct stat stat1, stat2;
    ASSERT_EQ(0, lstat((unpack_dir / "file1.txt").c_str(), &stat1));
    ASSERT_EQ(0, lstat((unpack_dir / "hardlink.txt").c_str(), &stat2));
    ASSERT_EQ(stat1.st_ino, stat2.st_ino);
}