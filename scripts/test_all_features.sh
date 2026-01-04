#!/bin/bash
# 全功能验证测试脚本

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "=========================================="
echo "   DataBackup 全功能验证测试"
echo "=========================================="
echo

# 设置库路径
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH

# 清理旧的测试文件
rm -rf test_all_* backup_*.dat

# 测试计数
TOTAL_TESTS=0
PASSED_TESTS=0

run_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "测试 $TOTAL_TESTS: $1 ... "
}

pass_test() {
    PASSED_TESTS=$((PASSED_TESTS + 1))
    echo -e "${GREEN}通过${NC}"
}

fail_test() {
    echo -e "${RED}失败${NC}"
    echo "  错误: $1"
}

# ============================================
# 测试 1: 基本打包和解包
# ============================================
run_test "基本打包和解包功能"
mkdir -p test_all_basic
echo "Hello World" > test_all_basic/file1.txt
echo "Test Data" > test_all_basic/file2.txt
mkdir -p test_all_basic/subdir
echo "Nested file" > test_all_basic/subdir/file3.txt

./data_backup pack test_all_basic backup_basic.dat > /dev/null 2>&1
./data_backup unpack backup_basic.dat test_all_basic_restored > /dev/null 2>&1

if diff -r test_all_basic test_all_basic_restored > /dev/null 2>&1; then
    pass_test
else
    fail_test "文件内容不一致"
fi

# ============================================
# 测试 2: 压缩功能
# ============================================
run_test "压缩功能"
# 创建一个可压缩的文件（重复内容）
mkdir -p test_all_compress
for i in {1..1000}; do
    echo "This is a repeating line for compression test" >> test_all_compress/compressible.txt
done

./data_backup pack test_all_compress backup_uncompressed.dat > /dev/null 2>&1
./data_backup pack test_all_compress backup_compressed.dat --compress > /dev/null 2>&1

SIZE_UNCOMPRESSED=$(stat -f%z backup_uncompressed.dat 2>/dev/null || stat -c%s backup_uncompressed.dat)
SIZE_COMPRESSED=$(stat -f%z backup_compressed.dat 2>/dev/null || stat -c%s backup_compressed.dat)

if [ $SIZE_COMPRESSED -lt $SIZE_UNCOMPRESSED ]; then
    ./data_backup unpack backup_compressed.dat test_all_compress_restored --compress > /dev/null 2>&1
    if diff test_all_compress/compressible.txt test_all_compress_restored/compressible.txt > /dev/null 2>&1; then
        pass_test
        echo "  压缩率: $SIZE_UNCOMPRESSED -> $SIZE_COMPRESSED 字节"
    else
        fail_test "解压后内容不一致"
    fi
else
    fail_test "压缩后文件大小没有减小"
fi

# ============================================
# 测试 3: 加密功能
# ============================================
run_test "加密和解密功能"
mkdir -p test_all_encrypt
echo "Secret data" > test_all_encrypt/secret.txt

./data_backup pack test_all_encrypt backup_encrypted.dat mypassword > /dev/null 2>&1

# 用正确密码解密
./data_backup unpack backup_encrypted.dat test_all_encrypt_restored mypassword > /dev/null 2>&1
if diff test_all_encrypt/secret.txt test_all_encrypt_restored/secret.txt > /dev/null 2>&1; then
    pass_test
else
    fail_test "解密后内容不一致"
fi

# ============================================
# 测试 4: 错误密码检测
# ============================================
run_test "错误密码检测"
rm -rf test_all_wrong_pass
if ./data_backup unpack backup_encrypted.dat test_all_wrong_pass wrongpassword > /dev/null 2>&1; then
    fail_test "应该拒绝错误的密码"
else
    pass_test
fi

# ============================================
# 测试 5: 软链接支持
# ============================================
run_test "软链接支持"
mkdir -p test_all_symlink
echo "original" > test_all_symlink/original.txt
ln -s original.txt test_all_symlink/link1
ln -s /etc/hosts test_all_symlink/link2

./data_backup pack test_all_symlink backup_symlink.dat > /dev/null 2>&1
./data_backup unpack backup_symlink.dat test_all_symlink_restored > /dev/null 2>&1

if [ -L test_all_symlink_restored/link1 ] && [ -L test_all_symlink_restored/link2 ]; then
    TARGET1=$(readlink test_all_symlink_restored/link1)
    TARGET2=$(readlink test_all_symlink_restored/link2)
    if [ "$TARGET1" = "original.txt" ] && [ "$TARGET2" = "/etc/hosts" ]; then
        pass_test
    else
        fail_test "软链接目标不正确"
    fi
else
    fail_test "软链接未正确恢复"
fi

# ============================================
# 测试 6: 硬链接支持
# ============================================
run_test "硬链接支持"
mkdir -p test_all_hardlink
echo "shared content" > test_all_hardlink/file1.txt
ln test_all_hardlink/file1.txt test_all_hardlink/file2.txt

INODE_BEFORE=$(ls -i test_all_hardlink/file1.txt | awk '{print $1}')

./data_backup pack test_all_hardlink backup_hardlink.dat > /dev/null 2>&1
./data_backup unpack backup_hardlink.dat test_all_hardlink_restored > /dev/null 2>&1

INODE1=$(ls -i test_all_hardlink_restored/file1.txt | awk '{print $1}')
INODE2=$(ls -i test_all_hardlink_restored/file2.txt | awk '{print $1}')

if [ "$INODE1" = "$INODE2" ]; then
    pass_test
else
    fail_test "硬链接未正确恢复（inode不同）"
fi

# ============================================
# 测试 7: 过滤功能 - include
# ============================================
run_test "过滤功能 (--include)"
mkdir -p test_all_filter
echo "txt file" > test_all_filter/file.txt
echo "log file" > test_all_filter/file.log
echo "cpp file" > test_all_filter/file.cpp

./data_backup pack test_all_filter backup_filter_include.dat --include "*.txt" > /dev/null 2>&1
./data_backup unpack backup_filter_include.dat test_all_filter_include > /dev/null 2>&1

if [ -f test_all_filter_include/file.txt ] && [ ! -f test_all_filter_include/file.log ]; then
    pass_test
else
    fail_test "include 过滤未生效"
fi

# ============================================
# 测试 8: 过滤功能 - exclude
# ============================================
run_test "过滤功能 (--exclude)"
./data_backup pack test_all_filter backup_filter_exclude.dat --exclude "*.log" > /dev/null 2>&1
./data_backup unpack backup_filter_exclude.dat test_all_filter_exclude > /dev/null 2>&1

if [ -f test_all_filter_exclude/file.txt ] && [ ! -f test_all_filter_exclude/file.log ]; then
    pass_test
else
    fail_test "exclude 过滤未生效"
fi

# ============================================
# 测试 9: 大小过滤
# ============================================
run_test "大小过滤 (--min-size)"
mkdir -p test_all_size
echo "small" > test_all_size/small.txt
head -c 10K /dev/zero > test_all_size/large.bin

./data_backup pack test_all_size backup_size.dat --min-size 1000 > /dev/null 2>&1
./data_backup unpack backup_size.dat test_all_size_restored > /dev/null 2>&1

if [ -f test_all_size_restored/large.bin ] && [ ! -f test_all_size_restored/small.txt ]; then
    pass_test
else
    fail_test "大小过滤未生效"
fi

# ============================================
# 测试 10: 元数据保留（权限）
# ============================================
run_test "元数据保留（权限）"
mkdir -p test_all_meta
echo "test" > test_all_meta/file.txt
chmod 754 test_all_meta/file.txt

./data_backup pack test_all_meta backup_meta.dat > /dev/null 2>&1
./data_backup unpack backup_meta.dat test_all_meta_restored > /dev/null 2>&1

PERM_ORIG=$(stat -c "%a" test_all_meta/file.txt 2>/dev/null || stat -f "%Lp" test_all_meta/file.txt)
PERM_REST=$(stat -c "%a" test_all_meta_restored/file.txt 2>/dev/null || stat -f "%Lp" test_all_meta_restored/file.txt)

if [ "$PERM_ORIG" = "$PERM_REST" ]; then
    pass_test
else
    fail_test "权限未正确恢复（$PERM_ORIG vs $PERM_REST）"
fi

# ============================================
# 测试 11: 压缩+加密组合
# ============================================
run_test "压缩和加密组合"
mkdir -p test_all_combo
for i in {1..100}; do
    echo "Combo test data line $i" >> test_all_combo/data.txt
done

./data_backup pack test_all_combo backup_combo.dat testpass --compress > /dev/null 2>&1
./data_backup unpack backup_combo.dat test_all_combo_restored testpass --compress > /dev/null 2>&1

if diff test_all_combo/data.txt test_all_combo_restored/data.txt > /dev/null 2>&1; then
    pass_test
else
    fail_test "组合功能失败"
fi

# ============================================
# 测试 12: 自动备份功能
# ============================================
run_test "自动备份功能"
mkdir -p test_all_auto_src test_all_auto_dest
echo "auto backup test" > test_all_auto_src/file.txt

./data_backup auto-backup --source=test_all_auto_src --dest=test_all_auto_dest --keep=3 --name=test > /dev/null 2>&1

if ls test_all_auto_dest/test-*.dat > /dev/null 2>&1; then
    pass_test
else
    fail_test "自动备份未生成文件"
fi

# ============================================
# 测试 13: 空目录处理
# ============================================
run_test "空目录处理"
mkdir -p test_all_empty/empty_dir

./data_backup pack test_all_empty backup_empty.dat > /dev/null 2>&1
./data_backup unpack backup_empty.dat test_all_empty_restored > /dev/null 2>&1

if [ -d test_all_empty_restored/empty_dir ]; then
    pass_test
else
    fail_test "空目录未恢复"
fi

# ============================================
# 测试 14: 中文文件名支持
# ============================================
run_test "中文文件名支持"
mkdir -p test_all_chinese
echo "中文内容" > "test_all_chinese/测试文件.txt"

if ./data_backup pack test_all_chinese backup_chinese.dat > /dev/null 2>&1; then
    ./data_backup unpack backup_chinese.dat test_all_chinese_restored > /dev/null 2>&1
    if [ -f "test_all_chinese_restored/测试文件.txt" ]; then
        pass_test
    else
        fail_test "中文文件名未恢复"
    fi
else
    fail_test "无法打包中文文件名"
fi

# ============================================
# 结果汇总
# ============================================
echo
echo "=========================================="
echo "   测试结果汇总"
echo "=========================================="
echo "总测试数: $TOTAL_TESTS"
echo -e "通过测试: ${GREEN}$PASSED_TESTS${NC}"
echo -e "失败测试: ${RED}$((TOTAL_TESTS - PASSED_TESTS))${NC}"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "\n${GREEN}✓ 所有测试通过！${NC}"
    SUCCESS=0
else
    echo -e "\n${YELLOW}⚠ 有测试失败，请检查${NC}"
    SUCCESS=1
fi

# 清理测试文件
echo
echo "清理测试文件..."
rm -rf test_all_* backup_*.dat

echo "完成！"
exit $SUCCESS

