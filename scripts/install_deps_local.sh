#!/bin/bash
# 在用户目录安装 OpenSSL 和 zlib（无需 sudo 权限）

set -e

# 颜色输出
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== 在用户目录安装依赖库 ===${NC}"
echo

# 设置安装目录
INSTALL_PREFIX="$HOME/.local"
SRC_DIR="$HOME/.local/src"

mkdir -p "$INSTALL_PREFIX"
mkdir -p "$SRC_DIR"

echo -e "${YELLOW}安装目录: $INSTALL_PREFIX${NC}"
echo

# 检查 zlib
echo "步骤 1: 检查 zlib..."
if [ -f "$INSTALL_PREFIX/lib/libz.a" ] || [ -f "$INSTALL_PREFIX/lib/libz.so" ]; then
    echo -e "${GREEN}✓ zlib 已安装${NC}"
else
    echo "正在下载并编译 zlib..."
    cd "$SRC_DIR"
    
    # 下载 zlib
    if [ ! -f "zlib-1.2.13.tar.gz" ]; then
        wget https://zlib.net/zlib-1.2.13.tar.gz || {
            echo -e "${RED}下载失败，尝试备用源...${NC}"
            wget https://github.com/madler/zlib/archive/refs/tags/v1.2.13.tar.gz -O zlib-1.2.13.tar.gz
        }
    fi
    
    # 解压并编译
    tar -xzf zlib-1.2.13.tar.gz
    cd zlib-1.2.13
    ./configure --prefix="$INSTALL_PREFIX"
    make -j$(nproc)
    make install
    
    echo -e "${GREEN}✓ zlib 安装完成${NC}"
fi

echo
echo "步骤 2: 检查 OpenSSL..."

if [ -f "$INSTALL_PREFIX/lib/libssl.a" ] || [ -f "$INSTALL_PREFIX/lib/libssl.so" ]; then
    echo -e "${GREEN}✓ OpenSSL 已安装${NC}"
else
    echo "正在下载并编译 OpenSSL..."
    cd "$SRC_DIR"
    
    # 下载 OpenSSL
    OPENSSL_VERSION="1.1.1w"
    if [ ! -f "openssl-${OPENSSL_VERSION}.tar.gz" ]; then
        wget https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz || {
            echo -e "${RED}下载失败，尝试备用源...${NC}"
            wget https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_1_1_1w.tar.gz -O openssl-${OPENSSL_VERSION}.tar.gz
        }
    fi
    
    # 解压并编译
    tar -xzf openssl-${OPENSSL_VERSION}.tar.gz
    cd openssl-${OPENSSL_VERSION}
    ./config --prefix="$INSTALL_PREFIX" --openssldir="$INSTALL_PREFIX/ssl" shared zlib
    make -j$(nproc)
    make install_sw  # 只安装软件，不安装文档（更快）
    
    echo -e "${GREEN}✓ OpenSSL 安装完成${NC}"
fi

echo
echo -e "${GREEN}=== 安装完成！ ===${NC}"
echo
echo "依赖库已安装到: $INSTALL_PREFIX"
echo
echo "接下来的步骤："
echo "1. 运行以下命令更新 Makefile："
echo "   ./update_makefile.sh"
echo
echo "2. 编译项目："
echo "   make clean && make"
echo
echo "注意：您可能需要将以下行添加到 ~/.bashrc 中："
echo "export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:\$LD_LIBRARY_PATH"
echo "export PKG_CONFIG_PATH=$INSTALL_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH"

