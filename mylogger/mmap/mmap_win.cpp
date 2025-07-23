/**
 * @file      mmap_win.cpp
 * @brief     [mmap win实现]
 * @author    Weijh
 * @version   1.0
 */

#include "mmap/mmap_aux.h"

#include <windows.h>
#include <iostream>

namespace logger {
    /// 自定义一个删除器
    class FileHandleDeleter {
    public:
        /// 重载()，方便让其可以像函数一样调用
        void operator()(HANDLE handle) {
            if (handle) {
                /// handle存在，调用底层api关闭
                CloseHandle(handle);
            }
        }
    };

    /// 利用unique+ptr管理HANDLE, 自定义删除器
    using FileHandlePtr = std::unique_ptr<std::remove_pointer_t<HANDLE>, FileHandleDeleter>;

    /**
     * @brief 尝试将文件映射到内存
     * @param capacity 映射的目标容量（字节）
     * @return 成功返回 true，失败返回 false
     * @note 映射区域包含头部和数据区
     * @note 需确保文件已创建且有足够大小
     */
    bool MMapAux::TryMap_(size_t capacity) {
        /// 打开或创建目标文件
        FileHandlePtr file_handler(CreateFileW(
                file_path_.wstring().c_str(),      ///
                GENERIC_READ | GENERIC_WRITE,       /// 读写权限
                0,                                  /// 不共享
                nullptr,
                OPEN_ALWAYS,                        /// 如果文件存在则打开，如果不存在则创建
                FILE_ATTRIBUTE_NORMAL,
                nullptr));

        /// 打开是否成功
        if (file_handler.get() == INVALID_HANDLE_VALUE) {
            return false;
        }

        /// 创建文件映射
        FileHandlePtr file_mapping_;
        file_mapping_.reset(CreateFileMapping(
                file_handler.get(),             /// CreateFileW获得的文件句柄
                nullptr,             /// 安全属性
                PAGE_READWRITE,              ///  映射页的保护模式
                0,                  ///  映射文件的最高位
                capacity,           ///  映射文件的最低位
                nullptr                          /// 映射对象的名称，用于跨进程共享
        ));
        if (!file_mapping_.get()) {
            DWORD err = GetLastError();
            std::cout << "CreateFileMapping 失败！错误码: " << err
                      << "，请求容量: " << capacity << " 字节" << std::endl;
            return false;
        }

        /// 调用 MapViewOfFile 将映射对象映射到进程的具体内存地址，后续通过内存指针读写文件内容
        handle_ = MapViewOfFile(file_mapping_.get(), FILE_MAP_ALL_ACCESS, 0, 0, capacity);

        if (!handle_) {
            DWORD err = GetLastError();
            std::cout << "MapViewOfFile 失败！错误码: " << err
                      << "，映射大小: " << capacity << " 字节" << std::endl;
            return false;
        }

        return true;
    }

    /**
     * @brief 释放当前内存映射
     * @note 调用 UnmapViewOfFile 解除内存区域与文件的映射关系. 解除映射后，handle_ 将置为 nullptr
     * @note 调用后无法再访问之前映射的数据
     */
    void MMapAux::Unmap_() {
        if (handle_) {
            UnmapViewOfFile(handle_);
        }
        handle_ = nullptr;
    }

    /**
    * @brief 调用 FlushViewOfFile 将指定范围的内存内容强制写入磁盘
    * @note 确保数据持久化到物理存储
    * @param        handle_：映射区域的起始地址
     *              capacity_：需要同步的字节数（通常为整个映射区域）
    */
    void MMapAux::Sync_() {
        if (handle_) {
            /// 强制将内存内容写入磁盘
            FlushViewOfFile(handle_, capacity_);
        }
    }
}