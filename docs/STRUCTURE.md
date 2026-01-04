# 项目文件结构说明

## 📂 根目录文件

```
data-backup/
├── README.md          ⭐ 主文档（从这里开始）
├── Makefile           🔧 编译配置
├── data_backup        🚀 主程序（编译后生成）
├── create.sh          📝 项目初始化脚本
└── gitignore          🙈 Git忽略规则
```

## 📂 核心目录

### src/ - 源代码
```
src/
├── main.cpp           # 程序入口，命令行参数解析
├── backup.cpp/h       # 核心功能：打包和解包
├── crypto.cpp/h       # 加密解密（AES-256）
├── compress.cpp/h     # 压缩解压（zlib）
├── filter.cpp/h       # 过滤规则（文件名、大小等）
├── auto_backup.cpp    # 自动备份和清理
└── archive.h          # 归档格式定义
```

**代码行数**: ~1500行  
**语言**: C++17

### scripts/ - 辅助脚本
```
scripts/
├── README.md                  # 脚本详细说明
├── run_backup.sh             ⭐ 运行包装脚本（最常用）
├── test_all_features.sh      ✅ 全功能测试
├── setup_conda_env.sh        🐍 Conda环境设置
├── install_deps_local.sh     📦 本地编译依赖
└── update_makefile.sh        🔧 Makefile更新
```

**推荐**: 日常使用 `run_backup.sh`

### docs/ - 文档
```
docs/
├── INSTALL.md          # 详细安装指南
├── QUICK_START.md      # 5分钟快速上手
├── PROJECT_STATUS.md   # 项目完成状态报告
└── STRUCTURE.md        # 本文件
```

### test/ - 测试
```
test/
└── test_main.cpp       # 单元测试（使用googletest）
```

### lib/ - 第三方库
```
lib/
└── googletest/         # Google测试框架（git submodule）
```

## 📊 文件大小统计

| 类型 | 数量 | 总大小 |
|------|------|--------|
| 源代码 (.cpp/.h) | 11 | ~30KB |
| 脚本 (.sh) | 5 | ~20KB |
| 文档 (.md) | 5 | ~50KB |
| 可执行文件 | 1 | 1.4MB |
| 测试文件 | 1 | ~5KB |

## 🎯 重要文件说明

### 必读文档
1. **README.md** - 完整使用说明，从这里开始
2. **docs/QUICK_START.md** - 快速上手指南
3. **scripts/README.md** - 脚本使用说明

### 核心代码
1. **src/backup.cpp** - 最核心的文件，实现打包/解包逻辑
2. **src/main.cpp** - 程序入口，理解命令行接口
3. **src/archive.h** - 理解归档格式

### 常用脚本
1. **scripts/run_backup.sh** - 日常使用
2. **scripts/test_all_features.sh** - 功能验证

## 🗑️ 可以删除的文件

如果空间紧张，以下文件可以安全删除：

### 临时文件
```bash
rm -rf backups/          # 测试生成的备份
rm -f *.dat              # 测试生成的归档文件
rm -f *.o                # 编译中间文件
```

### 可选脚本（如果不需要）
```bash
# 如果使用conda，这些脚本可以删除
rm scripts/install_deps_local.sh
rm scripts/update_makefile.sh

# 如果不需要独立环境
rm scripts/setup_conda_env.sh
```

### 文档（如果已经熟悉）
```bash
# 保留README.md，其他可选
rm docs/INSTALL.md
rm docs/QUICK_START.md
# 但建议保留PROJECT_STATUS.md作为参考
```

## 🔒 不要删除的文件

### 核心文件（删除会导致无法使用）
- ✅ `data_backup` - 主程序
- ✅ `src/*` - 所有源代码
- ✅ `Makefile` - 编译配置
- ✅ `scripts/run_backup.sh` - 运行脚本

### 重要文档
- ✅ `README.md` - 主文档
- ✅ `scripts/README.md` - 脚本说明

## 📏 磁盘占用

```
完整项目:  ~150MB (包含googletest)
核心文件:  ~2MB (不含googletest和文档)
最小配置:  ~1.5MB (仅可执行文件)
```

## 🔍 查找文件

### 按类型查找
```bash
# 所有源代码
find src -name "*.cpp" -o -name "*.h"

# 所有脚本
find scripts -name "*.sh"

# 所有文档
find . -name "*.md"
```

### 按大小查找
```bash
# 大于1MB的文件
find . -type f -size +1M

# 小于10KB的文件
find . -type f -size -10k
```

## 🎨 代码组织

### 模块划分
```
backup.cpp     → 核心逻辑 (400行)
crypto.cpp     → 加密模块 (180行)
compress.cpp   → 压缩模块 (120行)
filter.cpp     → 过滤模块 (90行)
auto_backup.cpp → 自动化 (60行)
main.cpp       → 入口 (130行)
```

### 依赖关系
```
main.cpp
  └─→ backup.h
       ├─→ crypto.h
       ├─→ compress.h
       ├─→ filter.h
       └─→ archive.h
```

## 📝 文件命名规范

- **源文件**: `lowercase.cpp`
- **头文件**: `lowercase.h`
- **脚本**: `snake_case.sh`
- **文档**: `UPPERCASE.md` 或 `Title_Case.md`
- **可执行文件**: `lowercase_with_underscore`

## 🔄 版本控制

### Git忽略的文件
```
*.o              # 编译中间文件
*.dat            # 备份文件
data_backup      # 可执行文件（可选）
test_*/          # 测试目录
*.tmp            # 临时文件
```

### 应该提交的文件
```
src/             # 所有源代码
scripts/         # 所有脚本
docs/            # 所有文档
Makefile         # 编译配置
README.md        # 主文档
```

## 📞 更多信息

- 完整功能说明: [../README.md](../README.md)
- 脚本使用: [../scripts/README.md](../scripts/README.md)
- 项目状态: [PROJECT_STATUS.md](PROJECT_STATUS.md)

