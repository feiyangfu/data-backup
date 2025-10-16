#include <gtest/gtest.h>
#include "backup.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>

namespace fs = std::filesystem;

// 辅助函数 compareDirectories (保持不变)
bool compareDirectories(const fs::path& p1, const fs::path& p2) {
    auto it1 = fs::recursive_directory_iterator(p1, fs::directory_options::none);
    auto it2 = fs::recursive_directory_iterator(p2, fs::directory_options::none);

    while (it1 != fs::recursive_directory_iterator{} && it2 != fs::recursive_directory_iterator{}) {
        if (fs::relative(*it1, p1) != fs::relative(*it2, p2)) return false;
        if (it1->is_directory() != it2->is_directory()) return false;
        // 因为我们移除了软链接，所以这个检查可以省略，但留着也无害
        if (it1->is_symlink() != it2->is_symlink()) return false;
        
        if (it1->is_regular_file()) {
             if (it1->file_size() != it2->file_size()) return false;
        }
        
        ++it1;
        ++it2;
    }
    return it1 == fs::recursive_directory_iterator{} && it2 == fs::recursive_directory_iterator{};
}


// --- 补全这个类 ---
class PackUnpackTest : public ::testing::Test {
protected:
    // SetUp 函数 (这是您修改后的正确版本)
    void SetUp() override {
        fs::create_directories(source_dir / "subdir");
        std::ofstream(source_dir / "file1.txt") << "hello";
        std::ofstream(source_dir / "subdir" / "file2.txt") << "world";
        
        // 只创建硬链接
        link((source_dir / "file1.txt").c_str(), (source_dir / "hardlink.txt").c_str());
    }

    // TearDown 函数 (保持不变)
    void TearDown() override {
        fs::remove_all(source_dir);
        fs::remove_all(unpack_dir);
        fs::remove(archive_file);
    }

    // --- 这是被意外删除的部分 ---
    // 成员变量的声明
    const fs::path source_dir = "test_source_dir";
    const fs::path unpack_dir = "test_unpack_dir";
    const fs::path archive_file = "test_archive.dat";
    // ----------------------------
};


// 测试用例 (保持不变)
TEST_F(PackUnpackTest, FullCycleValidation) {
    // 执行打包
    pack(source_dir, archive_file);
    ASSERT_TRUE(fs::exists(archive_file));

    // 执行解包
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