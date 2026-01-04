# 快速开始指南

## ✅ 已完成的安装步骤

1. ✅ 使用 conda 安装了 OpenSSL
2. ✅ 配置了 Makefile 以使用 conda 环境
3. ✅ 编译成功
4. ✅ 软链接功能测试通过

## 🚀 使用方法

### 方法1：设置环境变量（推荐）

在 `~/.bashrc` 中添加以下行：

```bash
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH
```

然后执行：
```bash
source ~/.bashrc
cd /data0/fufeiyang/src/data-backup
./data_backup pack source_dir backup.dat
```

### 方法2：使用包装脚本

```bash
cd /data0/fufeiyang/src/data-backup
./run_backup.sh pack source_dir backup.dat
```

### 方法3：临时设置（每次运行）

```bash
cd /data0/fufeiyang/src/data-backup
LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH ./data_backup pack source_dir backup.dat
```

## 📝 常用命令示例

### 基本备份和恢复
```bash
# 打包
./run_backup.sh pack /path/to/source backup.dat

# 解包
./run_backup.sh unpack backup.dat /path/to/restore
```

### 带压缩的备份
```bash
./run_backup.sh pack /path/to/source backup.dat --compress
./run_backup.sh unpack backup.dat /path/to/restore --compress
```

### 带加密的备份
```bash
./run_backup.sh pack /path/to/source backup.dat mypassword --compress
./run_backup.sh unpack backup.dat /path/to/restore mypassword --compress
```

### 过滤特定文件
```bash
# 只备份 .txt 文件
./run_backup.sh pack /path/to/source backup.dat --include "*.txt"

# 排除 .log 文件
./run_backup.sh pack /path/to/source backup.dat --exclude "*.log"

# 只备份大于 1MB 的文件
./run_backup.sh pack /path/to/source backup.dat --min-size 1048576
```

### 自动备份（定时任务）
```bash
./run_backup.sh auto-backup \
  --source=/home/user/data \
  --dest=/backup \
  --keep=7 \
  --compress=true
```

## 🔧 软链接支持

现已完全支持软链接！测试命令：
```bash
./test_symlink.sh
```

支持的软链接类型：
- ✅ 相对路径软链接
- ✅ 绝对路径软链接  
- ✅ 指向文件的软链接
- ✅ 指向目录的软链接

## 🐛 故障排除

### 问题：运行时提示找不到 libssl.so.3

**解决方案**：设置 LD_LIBRARY_PATH
```bash
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH
```

或使用 `run_backup.sh` 脚本，它会自动设置路径。

### 问题：需要重新编译

```bash
make clean
make data_backup
```

### 问题：conda 环境相关

确保您在正确的 conda 环境中：
```bash
conda activate base  # 或您使用的环境名
echo $CONDA_PREFIX   # 应该显示 conda 安装路径
```

## 📚 更多信息

- 完整功能说明：查看 `README.md`
- 安装指南：查看 `INSTALL.md`
- 测试脚本：`test_symlink.sh`

## 🎯 项目特点

✅ 完整的目录备份
✅ 文件压缩（zlib）
✅ AES-256 加密
✅ 元数据保留（权限、时间戳等）
✅ 硬链接识别
✅ **软链接支持**（新功能）
✅ 灵活的过滤规则
✅ 自动备份和旧数据清理
✅ 无需 sudo 权限运行

