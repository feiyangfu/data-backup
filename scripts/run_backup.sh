#!/bin/bash
# 运行 data_backup 的包装脚本，自动设置库路径

# 设置 conda 库路径
if [ -n "$CONDA_PREFIX" ]; then
    export LD_LIBRARY_PATH="$CONDA_PREFIX/lib:$LD_LIBRARY_PATH"
fi

# 执行主程序，传递所有参数
exec "$(dirname "$0")/data_backup" "$@"

