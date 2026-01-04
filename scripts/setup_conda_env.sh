#!/bin/bash
# 使用 conda 创建开发环境（推荐方法，无需 sudo）

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== 使用 Conda 设置开发环境 ===${NC}"
echo

# 检查 conda
if ! command -v conda &> /dev/null; then
    echo -e "${YELLOW}错误: 找不到 conda 命令${NC}"
    echo "请确保 conda 已正确安装并在 PATH 中"
    exit 1
fi

ENV_NAME="backup-dev"

echo "步骤 1: 检查环境是否存在..."
if conda env list | grep -q "^${ENV_NAME} "; then
    echo -e "${YELLOW}环境 '${ENV_NAME}' 已存在，将重新使用${NC}"
else
    echo "创建新的 conda 环境: ${ENV_NAME}"
    conda create -n ${ENV_NAME} -y
fi

echo
echo "步骤 2: 安装依赖库..."
conda install -n ${ENV_NAME} -c conda-forge -y \
    openssl \
    zlib \
    gcc_linux-64 \
    gxx_linux-64 \
    make

echo
echo -e "${GREEN}=== 安装完成！ ===${NC}"
echo
echo "接下来的步骤："
echo
echo "1. 激活 conda 环境："
echo -e "   ${YELLOW}conda activate ${ENV_NAME}${NC}"
echo
echo "2. 编译项目："
echo "   make clean && make"
echo
echo "3. 运行程序："
echo "   ./data_backup pack test_dir backup.dat"
echo
echo "4. 完成后退出环境："
echo "   conda deactivate"
echo
echo "提示: 您也可以将环境激活命令添加到 ~/.bashrc 中自动激活"

