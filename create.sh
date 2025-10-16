echo "--> 正在创建测试目录 'manual_source'..."
mkdir -p manual_source/subdir

# 2. 创建普通文件
echo "This is file one." > manual_source/file1.txt
echo "This is a file in a subdirectory." > manual_source/subdir/file2.txt

# 3. 创建硬链接
echo "--> 正在创建硬链接..."
ln manual_source/file1.txt manual_source/hardlink_to_file1.txt

echo "--> 测试环境创建完毕！"