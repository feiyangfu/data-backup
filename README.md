# DataBackup Tool

`DataBackup` 是一个为《软件开发综合实验》课程设计的、功能强大的命令行备份工具。它采用 C++ 编写，运行于 Linux 环境，能够将指定目录打包、压缩、加密成单个备份文件，并支持灵活的过滤规则和自动化定时备份。

## 核心功能

*   **一体化打包**：将复杂的目录结构完整地打包成一个单一的归档文件，便于传输和管理。
*   **高效压缩**：集成 `zlib` 库，使用 DEFLATE 算法有效减小备份文件体积。
*   **强力加密**：集成 `OpenSSL` 库，使用 AES-256-CBC 工业级标准加密备份数据，确保隐私安全。
*   **元数据支持**：完整保留文件的权限、所有者和修改时间戳，实现状态的精确克隆。
*   **硬链接感知**：能够正确识别和处理硬链接，避免数据冗余，节省备份空间。
*   **自定义备份**：提供强大的过滤引擎，允许用户根据文件名/路径、文件尺寸等规则精确筛选需要备份的内容。
*   **自动化调度**：支持与 Linux `systemd` Timers 或 `cron` 无缝集成，实现周期性自动备份和旧数据淘汰。

## 编译与安装

### 依赖项

在编译前，请确保您已安装以下开发库：

*   **g++ 编译器** (需支持 C++17)
*   **make**
*   **OpenSSL 开发库**: `sudo apt-get install libssl-dev`
*   **Zlib 开发库**: `sudo apt-get install zlib1g-dev`
*   **GTest (可选)**: 如果需要运行单元测试，请按照 `Makefile` 的说明将 googletest 添加为 `lib/` 目录下的子模块。

### 编译

在项目根目录下，直接运行 `make` 即可：
```bash
make
```
该命令会生成一个名为 `data_backup` 的可执行文件。

## 功能与测试指南

### 1. 备份与恢复 (以打包和解包的方式实现)

该功能将目录打包成 `.dat` 归档文件。

*   **命令格式**:
    ```bash
    ./data_backup pack <源目录> <目标归档文件>
    ./data_backup unpack <源归档文件> <目标目录>
    ```

*   **测试方法**:
    1.  创建测试目录：`mkdir -p test_dir/subdir && echo "hello" > test_dir/file.txt`
    2.  打包：`./data_backup pack ./test_dir ./my_backup.dat`
    3.  创建恢复目录：`mkdir restore_dir`
    4.  解包：`./data_backup unpack ./my_backup.dat ./restore_dir`
    5.  验证：`diff -rq test_dir restore_dir` (该命令应无任何输出)

### 2. 文件类型与元数据支持

工具能够正确处理多种文件属性。

*   **支持的文件类型**:
    *   ✅ 普通文件
    *   ✅ 目录
    *   ✅ 硬链接
    *   ❌ 软链接 (会被主动忽略)
    *   ❌ 设备文件、管道文件 (未支持)

*   **支持的元数据**:
    *   ✅ 文件权限 (mode)
    *   ✅ 文件所有者 (uid, gid) - 恢复时可能需要 `sudo` 权限
    *   ✅ 文件修改时间戳 (mtime)

*   **测试方法**:
    1.  创建测试文件：`echo "metadata" > meta.txt`
    2.  修改元数据：`chmod 754 meta.txt && touch -d "1 year ago" meta.txt`
    3.  查看原始元数据：`ls -l meta.txt`
    4.  将 `meta.txt` 放入一个目录，打包并解包。
    5.  查看恢复后的文件元数据：`ls -l <恢复目录>/meta.txt`
    6.  **验证**：对比前后两次 `ls -l` 的输出，权限和时间戳应保持一致。

### 3. 加密与解密

使用 AES-256 算法保护您的数据。密码将作为命令行参数提供。

*   **命令格式**:
    ```bash
    ./data_backup pack <源> <目标> <你的密码>
    ./data_backup unpack <源> <目标> <你的密码>
    ```

*   **测试方法**:
    1.  **加密打包**：`./data_backup pack ./test_dir ./encrypted.dat mySecretPass`
    2.  **用正确密码解密**：`./data_backup unpack ./encrypted.dat ./restore_dir mySecretPass`
    3.  **验证**：`diff -rq test_dir restore_dir` (应无输出)
    4.  **用错误密码解密**：`./data_backup unpack ./encrypted.dat ./restore_dir wrongPass`
    5.  **验证**：程序应输出 `Error: Invalid password.` 并失败。

### 4. 压缩与解压

使用 `zlib` 减小归档文件体积。通过 `--compress` 标志启用。

*   **命令格式**:
    ```bash
    ./data_backup pack <源> <目标> --compress
    ./data_backup unpack <源> <目标> --compress
    ```

*   **测试方法**:
    1.  创建可压缩文件：`echo "repeat repeat repeat" > test_dir/compressible.txt`
    2.  **不压缩打包**：`./data_backup pack ./test_dir ./uncompressed.dat`
    3.  **压缩打包**：`./data_backup pack ./test_dir ./compressed.dat --compress`
    4.  **验证**：`ls -lh uncompressed.dat compressed.dat` (压缩后的文件应明显更小)
    5.  **解压**：`./data_backup unpack ./compressed.dat ./restore_dir --compress`
    6.  **验证**：`diff -rq test_dir restore_dir` (应无输出)

### 5. 自定义备份 (过滤)

通过强大的过滤规则，精确控制备份内容。

*   **支持的选项**:
    *   `--include <pattern>`: 只包含匹配模式的文件 (白名单)。
    *   `--exclude <pattern>`: 排除匹配模式的文件或路径 (黑名单)。
    *   `--min-size <bytes>`: 文件大小下限。

*   **测试方法**:
    1.  创建测试环境：
        ```bash
        mkdir filter_lab
        echo "log data" > filter_lab/app.log
        echo "source code" > filter_lab/main.cpp
        echo "big data" > filter_lab/data.bin
        head -c 1024 /dev/urandom >> filter_lab/data.bin
        ```
    2.  **只备份 `.cpp` 文件**:
        `./data_backup pack ./filter_lab ./backup_cpp.dat --include "*.cpp"`
        (程序应输出 `Skipping` 其他文件的信息)
    3.  **备份除 `.log` 外的所有文件**:
        `./data_backup pack ./filter_lab ./backup_no_log.dat --exclude "*.log"`
    4.  **只备份大于1000字节的文件**:
        `./data_backup pack ./filter_lab ./backup_large.dat --min-size 1000`
        (程序应跳过 `app.log` 和 `main.cpp`)

### 6. 定时备份与数据淘汰

通过与 `systemd` Timers (推荐) 或 `cron` 集成，实现全自动备份。

*   **命令格式**:
    ```bash
    ./data_backup auto-backup --source=<路径> --dest=<路径> [AUTO_OPTIONS...]
    ```
*   **核心选项**:
    *   `--source`: 要备份的源目录 (必需)。
    *   `--dest`: 存放所有备份文件的目录 (必需)。
    *   `--keep`: 保留的最新备份数量 (默认: 5)。
    *   所有 `pack` 命令的过滤、压缩、加密选项也同样适用。

*   **测试方法 (手动)**:
    1.  创建自动备份目录：`mkdir my_auto_backups`
    2.  **第一次运行**：`./data_backup auto-backup --source=./test_dir --dest=./my_auto_backups --keep=2` (会创建一个备份文件)
    3.  **第二次运行**：(再次执行相同命令，会创建第二个备份文件)
    4.  **第三次运行**：(再次执行相同命令)
    5.  **验证**：程序应输出 `Deleting 1 old backup(s)...`，并删除掉第一个创建的备份文件。最终 `my_auto_backups` 目录中应只保留最新的两份备份。

*   **自动化部署 (使用 systemd Timers)**:
    1.  创建 `~/.config/systemd/user/databackup.service` 和 `~/.config/systemd/user/databackup.timer` 文件。
    2.  在 `.service` 文件中定义完整的 `auto-backup` 命令（所有路径必须为绝对路径）。
    3.  在 `.timer` 文件中用 `OnCalendar=` 定义执行周期（例如 `daily` 或 `hourly`）。
    4.  运行 `systemctl --user daemon-reload`，然后 `systemctl --user start databackup.timer`。
    5.  使用 `journalctl --user-unit databackup.service` 查看定时任务的输出日志。

