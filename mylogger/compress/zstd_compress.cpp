/**
 * @file      zstd_compress.cpp
 * @brief     [zstd压缩算法具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "compress/zstd_compress.h"

#include <cstring>

namespace logger {
    namespace compress {

        /**
         * @brief 构造函数，初始化压缩和解压缩上下文
         */
        ZstdCompress::ZstdCompress() {
            /// 创建压缩上下文和解压缩上下文
            cctx_ = ZSTD_createCCtx();
            /// 为了保证后面拿到的都是有效的指针，这里处理有效性
            if (!cctx_) {
                return ;
            }

            dctx_ = ZSTD_createDCtx();
            if (!dctx_) {
                ZSTD_freeCCtx(cctx_);       /// 释放已创建的压缩上下文
                cctx_ = nullptr;
                return;
            }
            /// 设置默认压缩级别(cctx_非空）
            ZSTD_CCtx_setParameter(cctx_, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
        }

        /**
         * @brief 释放压缩和解压缩上下文，避免资源泄漏
         */
        ZstdCompress::~ZstdCompress() {
            if (cctx_) {
                ZSTD_freeCCtx(cctx_);
            }
            if (dctx_) {
                ZSTD_freeDCtx(dctx_);
            }
        }

        /**
         * @brief 压缩流重置
         * 重置压缩上下文状态，用于新的压缩会话（避免重新创建上下文的开销）
         */
        void ZstdCompress::ResetStream() {
            if (cctx_) {
                /// ZSTD_reset_session_only标识仅重置会话状态（保留压缩级别等参数）
                ZSTD_CCtx_reset(cctx_, ZSTD_reset_session_only);
            }
        }

        /**
         * @brief 重置解压缩流
         * 重置解压缩上下文状态，用于新的解压缩会话（避免重新创建上下文的开销）
         */
        void ZstdCompress::ResetUncompressStream_()  {
            if (dctx_) {
                /// ZSTD_reset_session_only标识仅重置会话状态（保留压缩级别等参数）
                ZSTD_DCtx_reset(dctx_, ZSTD_reset_session_only);
            }
        }

        /**
         * @brief 判断是否为 ZSTD 压缩格式
         * @param input
         * @param input_size
         * @return
         */
        bool ZstdCompress::InternalIsCompressed(const void *input, size_t input_size) {
            if (!input || input_size < 4) {
                return false;
            }

            const uint8_t kMagicNumber[] = {0x28, 0xb5, 0x2f, 0xfd};
            return memcmp(input, kMagicNumber, sizeof(kMagicNumber)) == 0;
        }


        /**
         * @brief 将输入数据压缩到输出缓冲区，返回实际压缩大小。
         * @param input
         * @param input_size
         * @param output
         * @param output_size
         * @return 实际压缩后的字节数，若失败返回0
         */
        size_t ZstdCompress::Compress(const void *input, size_t input_size, void *output, size_t output_size) {
            /// 输入/输出缓冲区的有效性检查
            if (!input || input_size == 0 || !output || output_size == 0) {
                return 0;
            }

            /// ZSTD输入缓冲区（待压缩数据）
            ZSTD_inBuffer data = {input, input_size, 0};
            /// ZSTD输出缓冲区（压缩后数据存储）
            ZSTD_outBuffer out_buffer = {output, output_size, 0};
//            ZSTD_outBuffer out_buffer = {const_cast<void*>(reinterpret_cast<const void*>(output)),
//                                         output_size, 0};
            /// 调用流式压缩算法：ZSTD_compressStream2
            size_t ret = ZSTD_compressStream2(cctx_, &out_buffer, &data,
                                              ZSTD_e_flush);///ZSTD_e_flush：刷新模式，确保当前输入数据被完全压缩并写入输出缓冲区（适合单次压缩，非持续流式场景）。

            /// 检查压缩是否出错
            if (ZSTD_isError(ret) != 0) {
                return 0;
            }

            /// 返回实际压缩后的字节数（out_buffer.pos为已写入的字节数）
            return out_buffer.pos;
        }


        /**
         * @brief 使用 ZSTD 的流式解压缩接口 ZSTD_decompressStream 执行解压缩，支持分块处理压缩数据。
         * @param data
         * @param size
         * @return 解压缩后的字符串
         *
         */
        std::string ZstdCompress::Decompress(const void* data, size_t size) {
            if (!data || size == 0) {
                return "";
            }

            /// 强制重置解压缩流
            ResetUncompressStream_();
            /// dctx_上下文有效性检查
            if (!dctx_) {
                return "";
            }

            std::string output;

            /**
             *  reserve只是预分配空间，未进行初始化，不包含有效元素，不可写
             *  由于 size 未变（例如仍为 0），
             *  通过 [] 或 data() 访问预留空间的内存（如 vec[0] 或 str.data()[0]）属于**未定义行为**（可能崩溃或数据错乱）
             *
             * resize() 会为[0, n-1]范围内的元素分配内存并且初始化，这些内存是安全可写的
             * 可以通过 []， data()方式直接读写[0, N-1]范围内的元素，不会发生越界
             *
             *
            output.reserve(10 * 1024);  /// 预分配10KB缓冲区，避免频繁扩容

            /// const_cast<void*>(reinterpret_cast<const void*>(output.data()))是C++17之前的写法，因为std::string返回const char*
            /// 而 ZSTD_outBuffer结构体内部的dest是void*，必须要进行转换否则就会报错；还有一个原因是如果内部缓冲区不可写，通过const_cast会导致未定义行为
            ZSTD_outBuffer output_buffer = {const_cast<void*>(reinterpret_cast<const void*>(output.data())), output.capacity(),
                                            0};
            */

            /// 优化上述过程
            size_t estimated_size = ZSTD_getFrameContentSize(data, size);   /// 获取预估解压缩大小

            /// 初始化缓冲区，至少为10KB
            size_t initial_size = (estimated_size != ZSTD_CONTENTSIZE_ERROR && estimated_size != ZSTD_CONTENTSIZE_UNKNOWN )
                    ? estimated_size : 10 * 1024;

            output.resize(initial_size);

            ZSTD_outBuffer output_buffer = {output.data(), output.size(), 0};

            /// 定义ZSTD输入缓冲区（待解压缩数据）
            ZSTD_inBuffer input = {data, size, 0};
            size_t ret = ZSTD_decompressStream(dctx_, &output_buffer, &input);

            /// 循环处理缓冲区不足的情况（ret > 0 表示需要更多输出空间）
            while (ZSTD_isError(ret) == 0 && ret > 0) {
                output.resize(output.size() * 2);       /// 翻倍扩容
                output_buffer = {output.data(), output.size(), output_buffer.pos};
                ret = ZSTD_decompressStream(dctx_, &output_buffer, &input);
            }

            if (ZSTD_isError(ret) != 0) {
                return "";
            }

            /// 直接调整大小，避免冗余拷贝
            output.resize(output_buffer.pos);
            return output;
        }

        /**
         *
         * @param input_size
         * @return zstd压缩格式
         */
        size_t ZstdCompress::CompressedBound(size_t input_size) {
            return ZSTD_compressBound(input_size);
        }

    }
}