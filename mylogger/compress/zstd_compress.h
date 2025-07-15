/**
 * @file      zstd_compress.h
 * @brief     [zstd压缩方法实现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "compress/compress.h"

#include "third_party/zstd/lib/zstd.h"

namespace logger {
    namespace compress {

        /// zstd压缩算法，压缩率和速度均优于zlib
        class ZstdCompress final : public Compression {
        public:
            /// 构造函数/析构函数
            /// 初始化压缩和解压缩上下文
            ZstdCompress();

            /// 释放压缩和解压缩上下文，避免资源泄漏
            ~ZstdCompress() override;

            /// 将输入数据压缩到输出缓冲区，返回实际压缩大小
            size_t Compress(const void *input, size_t input_size, void *output, size_t output_size) override;

            /// 将压缩数据解压缩为字符串并返回
            std::string Decompress(const void *data, size_t size) override;

            /// 重置压缩上下文状态，用于重新开始压缩（避免重建上下文的开销）
            void ResetStream() override;

            /// 计算输入数据压缩后的最大可能大小（用于预分配输出缓冲区）
            size_t CompressedBound(size_t input_size) override;

            /// 判断数据是否压缩为zstd格式（对外的窗口）->调用内部的判断接口
            static bool IsCompressed(const void* input, size_t input_size) {
                return InternalIsCompressed(input, input_size);
            }


        private:
            void ResetUncompressStream_();

            /// 内部压缩状态判断接口
            static bool InternalIsCompressed(const void* input, size_t input_size);

            /// ZSTD 压缩上下文（ZSTD_CCtx*），用于管理压缩过程的状态（如压缩级别、字典等），复用上下文可提升性能
            ZSTD_CCtx *cctx_;
            /// ZSTD 解压缩上下文（ZSTD_DCtx*），用于管理解压缩过程的状态，复用可提升性能
            ZSTD_DCtx *dctx_;
        };

    }
}
