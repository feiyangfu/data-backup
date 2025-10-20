# 清理旧环境
rm -rf filter_lab

# 创建目录结构
mkdir -p filter_lab/src
mkdir -p filter_lab/build
mkdir -p filter_lab/assets/images

# 创建各种文件
echo "main function" > filter_lab/src/main.cpp
echo "backup logic" > filter_lab/src/backup.cpp
echo "temporary object file" > filter_lab/build/main.o
echo "final executable" > filter_lab/build/app
echo "Just a readme file" > filter_lab/README.md
echo "This is a very important log file from today." > filter_lab/server.log
echo "This is an old log file from yesterday." > filter_lab/server.log.old
echo "A small icon file" > filter_lab/assets/images/icon.png
echo "A larger image file, content doesn't matter, size matters." > filter_lab/assets/images/background.jpg
# 创建一个大于100字节的文件
head -c 200 /dev/urandom > filter_lab/large_file.dat

# 修改一个文件的时间戳为2天前
touch -d "2 days ago" filter_lab/server.log.old

echo "--> 详细测试环境创建完毕！"
ls -lR filter_lab # 查看一下我们的成果