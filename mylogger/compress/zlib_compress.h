/**
 * @file      zlib_compress.h
 * @brief     [zlib压缩/解压缩算法的具体实现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 继承compress抽象基类，并且导入zlib压缩算法库
#include "compress/compress.h"
#include "third_party/zlib/include/zlib.h"

namespace logger {
    namespace compress {
        /**
         * zlib 库中，压缩和解压缩需要通过 z_stream 结构体管理状态，且必须通过 deflateEnd（压缩）和 inflateEnd（解压缩）释放资源，
         * 否则会导致内存泄漏。代码通过 自定义删除器 结合 std::unique_ptr 实现自动资源管理
         * */
        struct ZStreamDeflateDeleter {      /// 用于压缩流（deflate）的删除器
            /// 重载operator()，仿函数
            void operator()(z_stream *stream) {
                if (stream) {
                    deflateEnd(stream);     /// 释放压缩流资源（zlib要求的清理函数）
                    delete stream;               /// 释放z_stream结构体自身的内存
                }
            }
        };

        /// 用于解压缩流（inflate）的删除器
        struct ZStreamInflateDeleter {
            void operator()(z_stream *stream) {
                if (stream) {
                    inflateEnd(stream);
                    delete stream;
                }
            }
        };

        /// Zlib压缩方法封装
        class ZlibCompress final : public Compression {
        public:
            ~ZlibCompress() = default;

            /// 添加构造函数，初始化流，保证每次压缩和解压缩，流都是初始化的
            ZlibCompress();

            /// 将输入数据压缩到输出缓冲区，返回压缩后的实际大小
            size_t Compress(const void *input, size_t input_size, void *output, size_t output_size) override;

            /// 计算输入数据压缩后的最大可能字节数
            size_t CompressedBound(size_t input_size) override;

            /// 将压缩数据解压缩并返回字符串形式的结果
            std::string Decompress(const void *data, size_t size) override;

            /// 重置压缩流的内部状态
            void ResetStream() override;

            /// 判断数据是否为zlib压缩格式（对外窗口）-->调用内部的判断接口
            static bool IsCompressed(const void* input, size_t input_size) {
                return InternalIsCompressed(input, input_size);
            }

        private:
            /// 重置解压缩流状态，类似 ResetStream，用于重新开始解压缩
            void ResetUncompressStream_();

            /// 内部静态函数
            static bool InternalIsCompressed(const void* input, size_t input_size);

            std::unique_ptr<z_stream, ZStreamDeflateDeleter> compress_stream_;          /// 管理压缩流的智能指针
            std::unique_ptr<z_stream, ZStreamInflateDeleter> uncompress_stream_;        /// 管理解压缩流的智能指针

        };


    }
}
