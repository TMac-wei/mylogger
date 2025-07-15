/**
 * @file      compress.h
 * @brief     [压缩算法抽象接口，具体的解压缩算法继承抽象接口并实现全部虚函数接口]
 * @author    Weijh
 * @version   1.0
 *
 */

#pragma once

#include <memory>
#include <string>

namespace logger {
    namespace compress {
        /// 该抽象类定义了压缩算法的通用行为，
        /// 允许实现不同的压缩策略（如 Gzip、Zlib、LZ4 等）。通过多态机制，用户可在运行时切换压缩算法，而无需修改上层代码。
        class Compression {
        public:
            virtual ~Compression() = default;

            /// 将输入数据压缩到输出缓冲区
            virtual size_t Compress(const void *input, size_t input_size, void *output, size_t output_size) = 0;

            /// 计算输入数据压缩后的最大可能字节数
            virtual size_t CompressedBound(size_t input_size) = 0;

            /// 将压缩数据解压缩并返回字符串形式的结果
            virtual std::string Decompress(const void *data, size_t size) = 0;

            /// 重置压缩流的内部状态
            virtual void ResetStream() = 0;
        };
    }
}
