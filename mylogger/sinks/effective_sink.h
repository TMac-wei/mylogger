/**
 * @file      effective_sink.h
 * @brief     [EffectiveSink-日志系统的核心组件]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "space.h"
#include "context/context.h"
#include "mmap/mmap_aux.h"
#include "compress/compress.h"
#include "crypt/crypt.h"
#include "sinks/sink.h"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <mutex>

namespace logger {
    namespace detail {
        /// 日志头结构体，包含魔数、大小、公钥信息
        struct LogChunkHeader {
            static constexpr uint64_t kMagic = 0xdeadbeefdada1100;
            uint64_t magic;
            uint64_t size;
            char pub_key[128];

            LogChunkHeader() : magic(kMagic), size(0) {}
        };

        /// 每个日志项结构体
        struct LogItemHeader {
            static constexpr uint32_t kMagic = 0xbe5fba11;
            uint32_t magic;
            uint32_t size;

            LogItemHeader() : magic(kMagic), size(0) {}
        };
    }

    class EffectiveSink final : public LogSink {
    public:

        /// 文件管理结构体
        struct Conf {
            std::filesystem::path file_dir_;            /// 文件目录
            std::string file_prefix_;                   /// 文件名前缀，最后需要将文件命名为：{file_prefix_}_{datetime}.log
            std::string pub_key_;                       /// 服务器公钥
            std::chrono::minutes interval{5};       /// 文件淘汰间隔，固定间隔
            megabytes single_file_size_{4};          /// 单个文件大小
            megabytes total_files_sizes_{100};       /// 所有文件大小
        };

        explicit EffectiveSink(Conf conf);

        ~EffectiveSink() override;

        /// 关键日志接口
        void Log(const LogMsg &msg) override;

        /// 设置格式化器
        void SetFormatter(std::unique_ptr<Formatter> formatter) override;

        /// 刷新
        void Flush() override;

    private:
        /// 私有接口函数
        /// 缓冲区切换
        void SwitchCache_();

        /// 判断是否需要从缓存写入文件
        bool NeedCacheToFile_();

        /// 写入缓存
        void WriteToCache_(const void *data, uint32_t size);

        /// 文件初始化准备阶段
        void PrepareFile_();

        /// 从缓存写入文件
        void CacheToFile_();

        /// 获取日志文件路径
        std::filesystem::path GetFilePath_();

        /// 结束
        void ElimateFiles_();

    private:
        Conf conf_;
        std::mutex mutex_;
        std::unique_ptr<Formatter> formatter_;                  /// 日志格式化器
        std::unique_ptr<MMapAux> master_cache_;                 /// 主缓冲区
        std::unique_ptr<MMapAux> slave_cache_;                  /// 从缓冲区
        std::unique_ptr<compress::Compression> compress_;      /// 压缩器
        std::unique_ptr<crypt::Crypt> crypt_;                    /// 加密器
        ctx::TaskRunnerTag task_runner_;                        /// 异步任务调度

        std::filesystem::path log_file_path_;                   /// 日志文件路径
        std::string client_pub_key_;                            /// 客户端公钥
        std::string compressed_buf_;                            /// 压缩输出缓冲区
        std::string encrypt_buf_;                               /// 加密输出缓冲区

        std::atomic<bool> is_slave_free_{true};             /// 从缓冲区是否可用，用于缓冲区切换
    };

}