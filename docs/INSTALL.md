# 安装指南

## 依赖项安装

### Ubuntu/Debian 系统

```bash
sudo apt-get update
sudo apt-get install -y libssl-dev zlib1g-dev
```

### CentOS/RHEL 系统

```bash
sudo yum install -y openssl-devel zlib-devel
```

### macOS 系统

```bash
brew install openssl zlib
```

## 编译步骤

### 1. 初始化 googletest (如果还没有)

```bash
git submodule update --init --recursive
```

### 2. 编译项目

```bash
make clean
make
```

### 3. 验证编译结果

```bash
./data_backup --help
```

## 测试

### 运行单元测试

```bash
make test
```

### 运行软链接功能测试

```bash
./test_symlink.sh
```

## 常见问题

### 问题：找不到 openssl/evp.h

**解决方案**：安装 libssl-dev
```bash
sudo apt-get install libssl-dev
```

### 问题：找不到 zlib.h

**解决方案**：安装 zlib1g-dev
```bash
sudo apt-get install zlib1g-dev
```

### 问题：googletest 目录为空

**解决方案**：初始化 git submodule
```bash
git submodule update --init --recursive
```

### 问题：Mac 上找不到 OpenSSL

**解决方案**：修改 Makefile 添加 OpenSSL 路径
```makefile
CXXFLAGS = -std=c++17 -Wall -g -I/usr/local/opt/openssl/include
LDFLAGS = -L/usr/local/opt/openssl/lib -lssl -lcrypto -lz
```

## 安装到系统

```bash
sudo make install
# 或者手动复制
sudo cp data_backup /usr/local/bin/
```

