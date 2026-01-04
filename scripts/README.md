# Scripts 目录说明

本目录包含项目相关的辅助脚本。

## 📁 脚本列表

### 1. run_backup.sh
**用途**: 主程序运行包装脚本  
**推荐度**: ⭐⭐⭐⭐⭐  
**说明**: 自动设置库路径并运行data_backup程序，最常用的脚本

**使用方法**:
```bash
./scripts/run_backup.sh pack source_dir backup.dat --compress
./scripts/run_backup.sh unpack backup.dat restore_dir --compress
```

---

### 2. test_all_features.sh
**用途**: 全功能验证测试  
**推荐度**: ⭐⭐⭐⭐  
**说明**: 运行14项功能测试，验证程序所有功能

**使用方法**:
```bash
cd /data0/fufeiyang/src/data-backup
./scripts/test_all_features.sh
```

**测试项目**:
- 基本打包/解包
- 压缩功能
- 加密/解密
- 错误密码检测
- 软链接支持
- 硬链接支持
- 过滤功能（include/exclude）
- 大小过滤
- 元数据保留
- 压缩+加密组合
- 自动备份
- 空目录处理
- 中文文件名

---

### 3. setup_conda_env.sh
**用途**: 创建独立的conda开发环境  
**推荐度**: ⭐⭐⭐  
**说明**: 创建名为backup-dev的conda环境并安装所有依赖

**使用方法**:
```bash
./scripts/setup_conda_env.sh
conda activate backup-dev
make
```

**适用场景**:
- 需要独立的开发环境
- 不想污染base环境
- 多个项目需要隔离

---

### 4. install_deps_local.sh
**用途**: 在用户目录编译安装依赖库  
**推荐度**: ⭐⭐  
**说明**: 从源码编译OpenSSL和zlib到~/.local目录

**使用方法**:
```bash
./scripts/install_deps_local.sh
./scripts/update_makefile.sh
make
```

**适用场景**:
- 没有conda环境
- 需要特定版本的依赖库
- 完全离线环境

**注意**: 编译时间较长（10-30分钟）

---

### 5. update_makefile.sh
**用途**: 更新Makefile以使用本地库  
**推荐度**: ⭐⭐  
**说明**: 配合install_deps_local.sh使用，修改Makefile指向~/.local

**使用方法**:
```bash
# 在运行install_deps_local.sh之后
./scripts/update_makefile.sh
```

---

## 🎯 推荐使用流程

### 场景1: 日常使用（最简单）
```bash
# 方法1: 设置环境变量（一次性）
echo 'export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc

# 然后直接使用
./data_backup pack source backup.dat

# 方法2: 使用包装脚本（无需设置）
./scripts/run_backup.sh pack source backup.dat
```

### 场景2: 首次安装
```bash
# 如果有conda（推荐）
conda install -c conda-forge openssl
make

# 如果没有conda
./scripts/install_deps_local.sh
./scripts/update_makefile.sh
make
```

### 场景3: 功能验证
```bash
# 快速测试
./scripts/test_all_features.sh

# 或使用简化版
./scripts/run_backup.sh pack test_dir test.dat
./scripts/run_backup.sh unpack test.dat restored
diff -r test_dir restored
```

---

## 📝 脚本维护说明

### 不需要的脚本
以下脚本在特定场景下可能用不到：
- `setup_conda_env.sh` - 如果已在base环境安装依赖
- `install_deps_local.sh` - 如果使用conda
- `update_makefile.sh` - 如果使用conda

### 核心脚本（不要删除）
- ✅ `run_backup.sh` - 最常用
- ✅ `test_all_features.sh` - 功能验证

---

## 🔧 故障排除

### 问题1: 脚本没有执行权限
```bash
chmod +x scripts/*.sh
```

### 问题2: 找不到库文件
```bash
# 使用run_backup.sh而不是直接运行data_backup
./scripts/run_backup.sh pack source backup.dat
```

### 问题3: conda环境问题
```bash
# 确认conda环境
echo $CONDA_PREFIX
# 应该显示conda安装路径

# 重新激活环境
conda deactivate
conda activate base
```

---

## 📞 更多帮助

- 查看主文档: `../README.md`
- 安装指南: `../docs/INSTALL.md`
- 快速开始: `../docs/QUICK_START.md`
- 项目状态: `../docs/PROJECT_STATUS.md`

