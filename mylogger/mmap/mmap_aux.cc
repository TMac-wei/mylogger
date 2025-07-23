/**
 * @file      mmap_aux.cc
 * @brief     [mmap具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "mmap_aux.h"

#include <string>

#include "utils/file_util.h"
#include "utils/sys_util.h"

namespace logger {

    /// 默认容量为512KB
    static constexpr size_t kDefaultCapacity = 512 * 1024;

    /**
         * @brief 构造函数，创建或打开一个内存映射文件
         * @param file_path 映射文件路径
         * @note 文件不存在时会自动创建
         * @note 默认初始容量为512KB
    */
    MMapAux::MMapAux(logger::MMapAux::fpath file_path)
            : file_path_(std::move(file_path)), handle_(nullptr), capacity_(0) {
        /// 获取文件大小
        size_t fs_size = fs::GetFileSize(file_path_);
        /// 获取目的大小，取默认大小与文件大小的最大值
        size_t dst_size = std::max(kDefaultCapacity, fs_size);
        /// 预分配
        Reserve_(dst_size);
        /// 初始化
        Init_();
    }

    /**
    * @brief 获取真实的数据地址
    * @note 获取真实的数据地址需要从结构头跳过头部长度
    */
    uint8_t *MMapAux::Data() const {
        if (!IsValid_()) {
            return nullptr;
        }
        /// 返回真实的数据的地址
        return static_cast<uint8_t *>(handle_) + sizeof(MMapHeader);
    }

    /**
    * @brief 获取mmap映射的大小
    * @note 头部结构体中的mmap_size_就是整个mmap的大小
    */
    size_t MMapAux::Size() const {
        if (!IsValid_()) {
            return 0;
        }
        return Header_()->mmap_size_;
    }

    /**
    * @brief 重置mmap大小
    * @note  根据获取到的最新有效值，进行重设，并且更新头部中的mmap_size_
    */
    void MMapAux::Resize(size_t new_size) {
        if (!IsValid_()) {
            return;
        }
        EnsureCapacity_(new_size);
        Header_()->mmap_size_ = new_size;
    }

    /**
    * @brief 清除mmap
    * @note  更新头部中的mmap_size_ = 0
    */
    void MMapAux::Clear() {
        if (IsValid_()) {
            return;
        }
        Header_()->mmap_size_ = 0;
    }

    /**
       * @brief 新的映射
       * @note 将data拷贝到Data（）返回的真实地址处
       */
    void MMapAux::Push(const void *data, size_t size) {
        if (IsValid_()) {
            size_t new_size = Size() + size;
            EnsureCapacity_(new_size);
            memcpy(Data() + Size(), data, size);
            /// 更新头部的 mmap_size_ 为新大小
            Header_()->mmap_size_ = new_size;
        }
    }

    /**
   * @brief 获取当前mmap占用情况
   * @note  已经使用的大小与capacity的比例
   */
    double MMapAux::GetRatio() const {
        if (!IsValid_()) {
            return 0.0;
        }
        return static_cast<double>(Size()) / (capacity_ - sizeof(MMapHeader));
    }

    /**
      * @brief 判断mmap是否为空
    */
    bool MMapAux::Empty() const {
        return Size() == 0;
    }

    /**
    * @brief 初始化内存映射头部
    * @note 检查并设置魔数(MMapHeader::kMagic)和初始大小(0),仅在创建或打开映射时调用
    */
    void MMapAux::Init_() {
        MMapHeader *header = Header_();
        if (!header) {
            return;
        }
        if (header->magic_ != MMapHeader::kMagic) {
            header->magic_ = MMapHeader::kMagic;
            header->mmap_size_ = 0;
        }
    }

    /**
     * @brief 将输入大小按系统页大小对齐，返回有效的内存映射容量，将用户指定的大小调整为符合要求的最小有效容量。
     * @param size 原始大小(字节)，需要进行对齐的输入值
     * @return 对齐后的有效容量(字节)，保证是页大小的整数倍
     * @note 实现原理：通过向上取整算法，计算大于等于原始大小的最小页对齐值
     * @note 依赖系统工具函数GetPageSize()获取当前系统的页大小
     */
    static size_t GetValidCapacity_(size_t size) {
        size_t page_size = GetPageSize();
        /// 向上取整算法：(n + m - 1) / m * m 得到大于等于n的最小m倍数
        return (size + page_size - 1) / page_size * page_size;
    }

    /**
     * @brief 预分配指定大小的内存映射空间
     * @param new_size 期望的新容量大小（字节）
     * @note 若新大小小于当前容量则不执行任何操作
     * @note 实际分配的容量会按系统页大小对齐
     */
    void MMapAux::Reserve_(size_t new_size) {
        if (new_size <= capacity_) {
            return;
        }
        new_size = GetValidCapacity_(new_size);
        if (new_size == capacity_) {
            return;
        }
        /// 释放原来的空间
        Unmap_();
        /// 更新对应的变量
        TryMap_(new_size);
        capacity_ = new_size;
    }

    /**
      * @brief 确保映射容量足够
      * @param new_size 所需的最小数据大小(字节)
      * @note 容量不足时会自动扩展
      * @note 扩展策略为按页大小递增
    */
    void MMapAux::EnsureCapacity_(size_t new_size) {
        size_t real_size = new_size + sizeof(MMapHeader);
        if (real_size <= capacity_) {
            return;
        }
        auto dst_capacity_ = capacity_;
        while (dst_capacity_ < real_size) {
            dst_capacity_ += GetPageSize();         /// 容量不够进行拓展
        }
        /// 分配dst_capacity_大小的空间
        Reserve_(dst_capacity_);
    }

    /**
       * @brief 获取容量
       */
    size_t MMapAux::Capacity_() const noexcept {
        return capacity_;
    }

    /**
     * @brief 验证映射有效性
     * @return 有效返回true，否则返回false
     * @note 通过检查魔数和映射状态验证
     */
    bool MMapAux::IsValid_() const {
        MMapHeader* header = Header_();
        if (!header) {
            return false;
        }
        /// 检查魔数是否被修改，验证是否有效
        return header->magic_ == MMapHeader::kMagic;
    }

    /**
     * @brief 获取内存映射头部指针
     * @return MmapHeader* 头部指针，无效时返回 nullptr
     * @note 需先确保映射有效且容量足够
     */
    MMapAux::MMapHeader *MMapAux::Header_() const {
        /// 如果header_为空则直接返回
        if (!handle_) {
            return nullptr;
        }
        if (capacity_ < sizeof(MMapHeader)) {
            return nullptr;
        }
        /// 返回映射头部指针
        return static_cast<MMapHeader *>(handle_);
    }

}
