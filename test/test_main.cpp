#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>

#include "../src/backup.h"

namespace fs = std::filesystem;

class FileCopyTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_path = "test_source.txt";
        dest_path = "test_dest.txt";
        test_content = "Hello C++ and GTest!\nThis is a test.";

        std::ofstream source_file(source_path);
        source_file << test_content;
        source_file.close();
    }

    void TearDown() override {
        fs::remove(source_path);
        fs::remove(dest_path);
    }

    fs::path source_path;
    fs::path dest_path;
    std::string test_content;
};


TEST_F(FileCopyTest, CorrectlyCopiesFileContent) {
    // 2. 执行 (Act)
    copyFileContent(source_path, dest_path);

    ASSERT_TRUE(fs::exists(dest_path));

    std::ifstream dest_file(dest_path);
    std::stringstream buffer;
    buffer << dest_file.rdbuf();
    std::string dest_content = buffer.str();

    ASSERT_EQ(test_content, dest_content);
}


TEST_F(FileCopyTest, HandlesNonExistentSourceFile) {

    fs::remove(source_path); 
    copyFileContent(source_path, dest_path);
    ASSERT_FALSE(fs::exists(dest_path)); 
}