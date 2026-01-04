# DataBackup - 命令行备份工具

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

一个功能强大的命令行备份工具，专为《软件开发综合实验》课程设计。支持压缩、加密、过滤和自动化备份。

---

## 📋 目录

- [特性](#-特性)
- [快速开始](#-快速开始)
- [安装](#-安装)
- [使用方法](#-使用方法)
- [命令详解](#-命令详解)
- [高级功能](#-高级功能)
- [项目结构](#-项目结构)
- [技术实现](#-技术实现)
- [测试验证](#-测试验证)
- [常见问题](#-常见问题)
- [贡献指南](#-贡献指南)

---

## ✨ 特性

### 核心功能
- 🗂️ **完整备份** - 保留目录结构、文件内容和元数据
- 🗜️ **高效压缩** - 使用zlib DEFLATE算法，压缩率可达90%+
- 🔐 **强力加密** - AES-256-CBC加密，PBKDF2密钥派生
- 🔗 **链接支持** - 完整支持硬链接和软链接
- 🎯 **智能过滤** - 支持文件名模式、大小、时间等多种过滤规则
- ⏰ **自动备份** - 定时备份和旧数据自动清理
- 📊 **元数据保留** - 保留文件权限、所有者、修改时间

### 文件类型支持
| 类型 | 支持 | 说明 |
|------|------|------|
| 普通文件 | ✅ | 完全支持 |
| 目录 | ✅ | 包括空目录 |
| 硬链接 | ✅ | 自动识别和恢复 |
| 软链接 | ✅ | 相对/绝对路径 |
| 设备文件 | ❌ | 不支持 |
| 命名管道 | ❌ | 不支持 |

---

## 🚀 快速开始

### 5分钟上手

```bash
# 1. 克隆或进入项目目录
cd /data0/fufeiyang/src/data-backup

# 2. 编译（如果还没编译）
make

# 3. 设置环境（添加到~/.bashrc以永久生效）
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH

# 4. 基本使用
./data_backup pack source_dir backup.dat
./data_backup unpack backup.dat restored_dir

# 5. 带压缩和加密
./data_backup pack source_dir backup.dat mypassword --compress
./data_backup unpack backup.dat restored_dir mypassword --compress
```

### 使用包装脚本（推荐）

```bash
# 无需设置环境变量，直接使用
./scripts/run_backup.sh pack source_dir backup.dat --compress
./scripts/run_backup.sh unpack backup.dat restored_dir --compress
```

---

## 📦 安装

### 前置要求

- **操作系统**: Linux (Ubuntu 18.04+, CentOS 7+)
- **编译器**: g++ 支持C++17
- **依赖库**: OpenSSL 1.1+, zlib 1.2+

### 方法1: 使用Conda（推荐，无需sudo）

```bash
# 安装依赖
conda install -c conda-forge openssl

# 编译
make

# 测试
./scripts/run_backup.sh pack test_dir test.dat
```

### 方法2: 系统包管理器（需要sudo）

```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev zlib1g-dev

# CentOS/RHEL
sudo yum install openssl-devel zlib-devel

# 编译
make
```

### 方法3: 本地编译（无需sudo，耗时较长）

```bash
# 从源码编译依赖到~/.local
./scripts/install_deps_local.sh

# 更新Makefile
./scripts/update_makefile.sh

# 编译
make
```

详细安装说明请查看 [docs/INSTALL.md](docs/INSTALL.md)

---

## 📖 使用方法

### 基本命令格式

```bash
data_backup <command> [arguments] [options]
```

### 三大核心命令

#### 1. pack - 打包备份

```bash
data_backup pack <源目录> <备份文件> [密码] [选项]
```

**示例**:
```bash
# 基本打包
./data_backup pack /home/user/data backup.dat

# 带压缩
./data_backup pack /home/user/data backup.dat --compress

# 带加密
./data_backup pack /home/user/data backup.dat mypassword

# 压缩+加密
./data_backup pack /home/user/data backup.dat mypassword --compress

# 过滤备份
./data_backup pack /home/user/data backup.dat --exclude "*.log" --min-size 1024
```

#### 2. unpack - 解包恢复

```bash
data_backup unpack <备份文件> <目标目录> [密码] [选项]
```

**示例**:
```bash
# 基本解包
./data_backup unpack backup.dat /home/user/restored

# 解密解包
./data_backup unpack backup.dat /home/user/restored mypassword --compress
```

#### 3. auto-backup - 自动备份

```bash
data_backup auto-backup --source=<源> --dest=<目标> [选项]
```

**示例**:
```bash
# 基本自动备份
./data_backup auto-backup --source=/home/user/data --dest=/backup

# 完整配置
./data_backup auto-backup \
  --source=/home/user/data \
  --dest=/backup \
  --keep=7 \
  --compress=true \
  --password=mypass \
  --exclude="*.tmp"
```

---

## 🎯 命令详解

### 打包选项 (pack)

| 选项 | 说明 | 示例 |
|------|------|------|
| `--compress` | 启用压缩 | `--compress` |
| `--include <pattern>` | 只包含匹配的文件 | `--include "*.txt"` |
| `--exclude <pattern>` | 排除匹配的文件 | `--exclude "*.log"` |
| `--min-size <bytes>` | 只备份大于指定大小的文件 | `--min-size 1024` |
| `<password>` | 加密密码（位置参数） | `mypassword` |

### 解包选项 (unpack)

| 选项 | 说明 | 示例 |
|------|------|------|
| `--compress` | 解压缩 | `--compress` |
| `<password>` | 解密密码（位置参数） | `mypassword` |

### 自动备份选项 (auto-backup)

| 选项 | 必需 | 默认值 | 说明 |
|------|------|--------|------|
| `--source=<path>` | ✅ | - | 源目录 |
| `--dest=<path>` | ✅ | - | 备份存储目录 |
| `--keep=<number>` | ❌ | 5 | 保留最新备份数量 |
| `--name=<basename>` | ❌ | backup | 备份文件名前缀 |
| `--password=<pass>` | ❌ | - | 加密密码 |
| `--compress=<true\|false>` | ❌ | false | 是否压缩 |
| `--include=<pattern>` | ❌ | - | 包含模式 |
| `--exclude=<pattern>` | ❌ | - | 排除模式 |

---

## 🔥 高级功能

### 1. 过滤规则

#### 文件名模式匹配
```bash
# 只备份文本文件
./data_backup pack source backup.dat --include "*.txt"

# 排除日志文件
./data_backup pack source backup.dat --exclude "*.log"

# 排除多个模式
./data_backup pack source backup.dat --exclude "*.log" --exclude "*.tmp"

# 排除目录
./data_backup pack source backup.dat --exclude "node_modules"
```

#### 文件大小过滤
```bash
# 只备份大于1MB的文件
./data_backup pack source backup.dat --min-size 1048576

# 只备份大于10KB的文件
./data_backup pack source backup.dat --min-size 10240
```

#### 组合过滤
```bash
# 只备份大于1KB的.cpp文件
./data_backup pack source backup.dat --include "*.cpp" --min-size 1024
```

### 2. 定时自动备份

#### 使用cron
```bash
# 编辑crontab
crontab -e

# 每天凌晨2点备份
0 2 * * * cd /data0/fufeiyang/src/data-backup && ./scripts/run_backup.sh auto-backup --source=/home/user/data --dest=/backup --keep=7 --compress=true
```

#### 使用systemd timer
```bash
# 创建服务文件
cat > ~/.config/systemd/user/backup.service << 'EOF'
[Unit]
Description=Data Backup Service

[Service]
Type=oneshot
WorkingDirectory=/data0/fufeiyang/src/data-backup
ExecStart=/data0/fufeiyang/src/data-backup/scripts/run_backup.sh auto-backup --source=/home/user/data --dest=/backup --keep=7 --compress=true
EOF

# 创建定时器
cat > ~/.config/systemd/user/backup.timer << 'EOF'
[Unit]
Description=Daily Backup Timer

[Timer]
OnCalendar=daily
Persistent=true

[Install]
WantedBy=timers.target
EOF

# 启用并启动
systemctl --user daemon-reload
systemctl --user enable backup.timer
systemctl --user start backup.timer

# 查看状态
systemctl --user status backup.timer
```

### 3. 软链接和硬链接

```bash
# 创建测试环境
mkdir test_links
echo "original" > test_links/file.txt
ln -s file.txt test_links/symlink          # 软链接
ln test_links/file.txt test_links/hardlink # 硬链接

# 备份（自动处理链接）
./data_backup pack test_links backup.dat

# 恢复（链接关系保留）
./data_backup unpack backup.dat restored

# 验证
ls -li restored/  # 查看inode，硬链接应相同
readlink restored/symlink  # 查看软链接目标
```

### 4. 加密最佳实践

```bash
# ❌ 不推荐：密码在命令行可见
./data_backup pack source backup.dat mypassword

# ✅ 推荐：从环境变量读取（需要修改代码支持）
export BACKUP_PASSWORD="mypassword"

# ✅ 推荐：使用密码文件（需要修改代码支持）
echo "mypassword" > ~/.backup_pass
chmod 600 ~/.backup_pass

# 🔒 安全提示
# 1. 使用强密码（至少16字符，包含大小写字母、数字、符号）
# 2. 不要在脚本中硬编码密码
# 3. 定期更换密码
# 4. 备份文件和密码分开存储
```

---

## 📁 项目结构

```
data-backup/
├── README.md                 # 本文件
├── Makefile                  # 编译配置
├── data_backup               # 主程序（编译后）
├── create.sh                 # 项目创建脚本
│
├── src/                      # 源代码
│   ├── main.cpp             # 程序入口
│   ├── backup.cpp/h         # 打包/解包核心逻辑
│   ├── crypto.cpp/h         # 加密/解密实现
│   ├── compress.cpp/h       # 压缩/解压实现
│   ├── filter.cpp/h         # 过滤规则实现
│   ├── auto_backup.cpp      # 自动备份实现
│   └── archive.h            # 归档格式定义
│
├── test/                     # 单元测试
│   └── test_main.cpp
│
├── lib/                      # 第三方库
│   └── googletest/          # 测试框架
│
├── scripts/                  # 辅助脚本
│   ├── README.md            # 脚本说明
│   ├── run_backup.sh        # 运行包装脚本（推荐使用）
│   ├── test_all_features.sh # 全功能测试
│   ├── setup_conda_env.sh   # Conda环境设置
│   ├── install_deps_local.sh # 本地编译依赖
│   └── update_makefile.sh   # Makefile更新
│
└── docs/                     # 文档
    ├── INSTALL.md           # 安装指南
    ├── QUICK_START.md       # 快速开始
    └── PROJECT_STATUS.md    # 项目状态报告
```

---

## 🔧 技术实现

### 归档格式

```
[ArchiveHeader] [FileData] [Padding] [ArchiveHeader] [FileData] [Padding] ...
```

- **块大小**: 512字节对齐（类似tar）
- **头部结构**: 固定大小，包含文件名、权限、大小等
- **数据存储**: 紧跟在头部之后，按512字节对齐

### 加密流程

```
原始数据 → 打包 → [可选]压缩 → [可选]加密 → 备份文件
```

1. **密钥派生**: PBKDF2-HMAC-SHA256, 10000次迭代
2. **加密算法**: AES-256-CBC
3. **随机盐**: 每次加密使用新的16字节随机盐
4. **密码验证**: 存储密码哈希用于验证

### 压缩算法

- **算法**: zlib DEFLATE
- **级别**: 默认压缩级别（Z_DEFAULT_COMPRESSION）
- **缓冲区**: 16KB块大小
- **压缩率**: 文本文件可达90%+，二进制文件30-50%

### 链接处理

- **硬链接**: 通过inode识别，第一次存储完整数据，后续只存储引用
- **软链接**: 使用`readlink()`读取目标，使用`symlink()`恢复
- **跨文件系统**: 软链接支持，硬链接不支持

---

## ✅ 测试验证

### 运行测试

```bash
# 全功能测试（推荐）
./scripts/test_all_features.sh

# 软链接专项测试
./scripts/test_symlink.sh

# 手动测试
./scripts/run_backup.sh pack test_dir test.dat --compress
./scripts/run_backup.sh unpack test.dat restored --compress
diff -r test_dir restored
```

### 测试覆盖

| 测试项 | 状态 | 说明 |
|--------|------|------|
| 基本打包/解包 | ✅ | 目录结构完整性 |
| 压缩功能 | ✅ | 压缩率验证 |
| 加密/解密 | ✅ | 密码正确性 |
| 错误密码检测 | ✅ | 安全性验证 |
| 软链接 | ✅ | 相对/绝对路径 |
| 硬链接 | ✅ | inode一致性 |
| 过滤规则 | ✅ | include/exclude |
| 元数据保留 | ✅ | 权限/时间戳 |
| 自动备份 | ✅ | 定时和清理 |
| 中文文件名 | ✅ | UTF-8支持 |

---

## ❓ 常见问题

### Q1: 运行时提示找不到libssl.so.3

**A**: 需要设置库路径或使用包装脚本

```bash
# 方法1: 设置环境变量（推荐）
export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH

# 方法2: 使用包装脚本
./scripts/run_backup.sh pack source backup.dat
```

### Q2: 编译时找不到openssl/evp.h

**A**: 需要安装OpenSSL开发库

```bash
# 使用conda（推荐）
conda install -c conda-forge openssl

# 或使用apt（需要sudo）
sudo apt-get install libssl-dev
```

### Q3: 密码错误但程序没有报错？

**A**: 已修复，当前版本会正确返回错误码。如果仍有问题，请重新编译：

```bash
make clean && make
```

### Q4: 可以备份多大的文件？

**A**: 
- 理论上限: 2^64字节
- 推荐上限: 单个文件不超过100MB
- 大文件建议: 使用压缩减小体积

### Q5: 支持增量备份吗？

**A**: 当前版本不支持增量备份，每次都是全量备份。可以使用auto-backup的--keep参数保留多个版本。

### Q6: 可以在Mac上使用吗？

**A**: 核心功能兼容，但需要：
1. 安装依赖: `brew install openssl zlib`
2. 修改Makefile添加库路径
3. 定时任务使用launchd而不是systemd

### Q7: 如何查看备份文件内容而不解包？

**A**: 当前版本不支持，需要完整解包。这是一个可以改进的功能。

### Q8: 备份文件可以在不同机器间移动吗？

**A**: 可以，但注意：
- 加密备份需要相同的密码
- 绝对路径软链接可能失效
- 文件所有者UID/GID可能不同

---

## 🤝 贡献指南

### 报告问题

如果发现bug或有功能建议，请：
1. 检查是否已有相关issue
2. 提供详细的复现步骤
3. 包含系统信息和错误日志

### 代码贡献

1. Fork本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

### 代码规范

- 使用C++17标准
- 遵循现有代码风格
- 添加必要的注释
- 更新相关文档

---

---

**⭐ 如果这个项目对你有帮助，请给个Star！**

---

*最后更新: 2026-01-04*
