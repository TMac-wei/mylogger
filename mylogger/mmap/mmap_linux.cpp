/**
 * @file      mmap_linux.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "mmap/mmap_aux.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "defer.h"

namespace logger {

    /**
    * @brief 尝试将文件映射到内存
    * @param capacity 映射的目标容量（字节）
    * @return 成功返回 true，失败返回 false
    * @note 映射区域包含头部和数据区
    * @note 需确保文件已创建且有足够大小
    */
    bool MMapAux::TryMap_(size_t capacity) {
        /// 打开或创建文件
        int fd = open(file_path_.string().c_str(), O_RDWR | O_CREAT, S_IRWXU);
        /// 采用defer机制，如果错误自动释放
        LOG_DEFER {
            if (fd != -1) {
                /// 作用域结束时自动关闭文件描述符
                close(fd);
            }
        };

        if (fd == -1) {
            /// 打开文件失败
            return false;
        } else {
            /// 调整文件大小为指定容量
            ftruncate(fd, capacity);
        }

        /// 创建内存映射
        handle_ = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        return handle_ != MAP_FAILED;
    }

    /**
     * @brief 释放当前内存映射
     * @note 解除映射后，handle_ 将置为 nullptr
     * @note 调用后无法再访问之前映射的数据
     */
    void MMapAux::Unmap_() {
        if (handle_) {
            munmap(handle_, capacity_);
        }
        handle_ = nullptr;
    }

    /**
     * @brief 将内存中的数据同步到磁盘
     * @note 确保数据持久化到物理存储
     * @note 调用系统 msync 实现
     */
    void MMapAux::Sync_() {
        if (handle_) {
            msync(handle_, capacity_, MS_SYNC);
        }
    }
}