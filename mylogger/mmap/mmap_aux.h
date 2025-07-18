/**
 * @file      mmap_aux.h
 * @brief     [实现mmap内存映射]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <filesystem>
#include <memory>

namespace logger {

    /**
     * @brief 内存映射文件辅助类
     *
     * 提供基于mmap的文件数据管理功能，支持自动容量扩展、数据有效性验证和高效读写操作
     */

    class MMapAux {
    public:
        /// 定义文件系统别名
        using fpath = std::filesystem::path;

        /**
         * @brief 构造函数，创建或打开一个内存映射文件
         * @param file_path 映射文件路径
         * @note 文件不存在时会自动创建
         * @note 默认初始容量为512KB
         */
        explicit MMapAux(fpath file_path);

        /// 禁止拷贝和赋值
        MMapAux(const MMapAux &) = delete;

        MMapAux &operator=(const MMapAux &) = delete;

        /**
         * @brief 析构函数，自动释放内存映射
         */
        ~MMapAux() = default;

        /// 以下是接口方法
        uint8_t *Data() const;

        size_t Size() const;

        void Resize(size_t new_size);

        void Push(const void *data, size_t size);

        double GetRatio() const;

        bool Empty() const;

        void Clear();


        bool IsValid_() const;

    private:
        /// mmap结构头，包含魔数+size
        struct MMapHeader {
            static constexpr uint32_t kMagic = 0xdeadbeef;
            uint32_t magic_ = kMagic;
            uint32_t mmap_size_;
        };

        /// 私有接口方法
        void Reserve_(size_t new_size);

        void EnsureCapacity_(size_t new_size);

        size_t Capacity_() const noexcept;

        /// 尝试将文件映射到内存，直接在win或者Linux下实现
        bool TryMap_(size_t capacity);

        void Unmap_();

        void Sync_();

        MMapHeader* Header_() const;

        void Init_();


        fpath file_path_;       /// 文件路径
        void *handle_;
        size_t capacity_;


    };
}