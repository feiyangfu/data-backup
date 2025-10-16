#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <cstdint> 
#include <sys/stat.h> 

const int BLOCK_SIZE = 512;

struct ArchiveHeader {
    char name[256];      // 文件或目录的相对路径
    char link_target[256]; // 如果是软链接或硬链接，这里是目标路径
    mode_t mode;         // 文件类型和权限
    uid_t uid;           // 用户ID
    gid_t gid;           // 组ID
    uint64_t size;       // 文件大小（字节）
    time_t mtime;        // 最后修改时间
};

#endif // ARCHIVE_H