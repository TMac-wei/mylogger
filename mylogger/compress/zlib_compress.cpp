/**
 * @file      zlib_compress.cpp
 * @brief     [Zlib压缩具体实现]
 * @author    Weijh
 * @version   1.0
 */
#include "compress/zlib_compress.h"

namespace logger {
    namespace compress {

        /**
         * @brief 构造函数，保证压缩流和解压缩流都被初始化
         */
        ZlibCompress::ZlibCompress() {
            ResetStream();              /// 压缩流初始化
            ResetUncompressStream_();   /// 解压缩流初始化
        }


        /**
         * @brief 核心辅助函数，判断输入数据是否是zlib压缩格式，用于解压缩前的格式校验
         * @param input ,input_size
         * @return true or false
         *      zlib压缩数据的前两个字节（CMF与FLG字段）构成魔数，标识压缩格式，检查前两个字节是否匹配zlib标准魔数判断是否为压缩数据
         *  魔数对应的级别
         *      0x9c78：对应默认压缩级别（Z_DEFAULT_COMPRESSION）
         *      0xda78：对应最高压缩级别
         *      0x5e78：对应最低压缩级别
         *      0x0178：特殊格式（如原始 DEFLATE 流）
         * */
        bool ZlibCompress::InternalIsCompressed(const void *input, size_t input_size) {
            /// 如果输入为空，并且输入长度小于2，都不是zlib压缩数据
            if (!input || input_size < 2) {
                return false;
            }

            /// 如果输入字段的魔数符合上述魔数级别标准，则为zlib压缩数据
            /// uint16_t magic_num = *(uint16_t *) input;  /// 这种方式可能会存在未对齐访问的问题，只用memcpy安全的获取前两个字节
            uint16_t magic_num;
            memcpy(&magic_num, input, sizeof(magic_num));   /// 不依赖对齐，安全读取
            if (magic_num == 0x9c78 || magic_num == 0xda78 || magic_num == 0x5e78 || magic_num == 0x0178) {
                return true;
            }
            return false;
        }

        /**
           * @brief 将输入数据压缩到输出缓冲区，返回压缩后的实际大小（字节数）
           * @param input ,input_size, output, output_size
           * @return 压缩后的实际字节数
           *       实现步骤：
           *       1、参数校验：若输入/输出指针为空，直接返回0
           *       2、压缩流检查，若compress_stream_未初始化（为空），返回0（需要先调用ResetStream初始化）
           *       3、设置zlib输入输出：
           *           next_in:输入数据指针（转换为 Bytef*，zlib 定义的字节指针类型）
           *           avail_in：输入数据大小（转换为 uInt，zlib 定义的无符号整数类型）
           *           next_out：输出缓冲区指针
           *           avail_out：输出缓冲区大小
           *       4、开始压缩
           *       5、返回结果
         * */
        size_t ZlibCompress::Compress(const void *input, size_t input_size, void *output, size_t output_size) {
            if (!input || !output) {
                /// 后期考虑优化不同的错误码，明确是哪种类型的错误
                return 0;
            }

            if (!compress_stream_) {
                /// 这里其实还可以添加错误处理逻辑，暂时先不添加，后期添加
                return 0;
            }

            /// 每次压缩前重置压缩流状态（保留初始配置，仅重置数据）
            int ret1 = deflateReset(compress_stream_.get());
            if (ret1 != Z_OK) {
                return 0;   /// 重置失败，返回错误
            }

            /// 设置zlib输入输出
            compress_stream_->next_in = (Bytef *) input;
            compress_stream_->avail_in = static_cast<uInt>(input_size);

            compress_stream_->next_out = (Bytef *) output;
            compress_stream_->avail_out = static_cast<uInt>(output_size);

            int ret = Z_OK;
            /// 开始压缩
            do {
                ret = deflate(compress_stream_.get(), Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
                    return 0;
                }
            } while (ret == Z_BUF_ERROR);

            /// 处理返回值:实际压缩后的字节数 = 输出缓冲区总大小 - 剩余可用空间
            size_t out_len = output_size - compress_stream_->avail_out;
            return out_len;
        }

        /**
         * @brief 计算输入数据压缩后的最大可能字节数
         * @param input_size
         * @return 压缩后的最大可能大小
         *          准确公式为：input_size + (input_size / 1000) + 12;
         *          我这里修改为直接调用zlib库中的compressBound
         */
        size_t ZlibCompress::CompressedBound(size_t input_size) {
            /// 这里直接调用 zlib自带的compressBound函数
//            return input_size + (input_size / 1000) + 12;
            return compressBound(static_cast<uLong>(input_size));
        }

        /**
         * @brief 将压缩数据解压缩并返回字符串形式的结果
         * @param data  输入指针
         * @param size  输入大小
         * @return  数据解压缩之后的字符串形式的结果
         *      实现步骤：
         *       1、 参数校验：输入指针为空时，返回空字符串
         *       2、 调用 ResetUncompressStream_ 重置解压缩流
         *       3、 解压缩检查： 如果uncompress_stream_未初始化，返回空字符串
         *       4、 设置输入参数：next_in（压缩数据指针）、avail_in（压缩数据大小）
         *       5、 循环解压缩
         *       6、 返回结果
         */
        std::string ZlibCompress::Decompress(const void *data, size_t size) {
            if (!data) {
                return "";
            }
            /// 每次解压缩前都重置流
            ResetUncompressStream_();

            if (!uncompress_stream_) {
                return "";
            }

            /// 设置输入参数
            uncompress_stream_->next_in = (Bytef *) data;
            uncompress_stream_->avail_in = static_cast<uInt>(size);

            std::string output;
            /// 循环解压缩
            while (uncompress_stream_->avail_in > 0) {
                /// 分配4096字节临时缓冲区
                char buffer[4096] = {0};
                /// 分别设置 next_out（缓冲区指针）、avail_out（缓冲区大小）
                uncompress_stream_->next_out = (Bytef *) buffer;
                uncompress_stream_->avail_out = sizeof(buffer);
                /// inflate zlib核心解压缩函数，使用Z_SYNC_FLUSH模式
                int ret = inflate(uncompress_stream_.get(), Z_SYNC_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    return "";
                }
                /// 临时缓冲区中的有效数据追加到输出字符串
                output.append(buffer, sizeof(buffer) - uncompress_stream_->avail_out);
            }
            return output;
        }

        /**
         * @brief 初始化或重置压缩 / 解压缩流（z_stream），用于新的压缩 / 解压缩会话（避免重复初始化的开销）
         * 实现过程：
         *      1、创建新的z_stream实例，由unique_ptr统一管理（自定义删除器模式）
         *      2、初始化各个字符
         *          zalloc : 内存分配函数，ZNULL为默认
         *          zfree : 内存释放函数，ZNULL为默认
         *          opaque : 用户数据，ZNULL为默认
         *      3、初始化压缩流 deflateInit2 常见参数用时自己查
         *      4、初始化失败，重置compress_stream_（调用reset())
         */
        void ZlibCompress::ResetStream() {
            /// 压缩流重置
            compress_stream_ = std::unique_ptr<z_stream, ZStreamDeflateDeleter>(new z_stream());
            compress_stream_->zalloc = Z_NULL;
            compress_stream_->zfree = Z_NULL;
            compress_stream_->opaque = Z_NULL;

            /// 初始化压缩流
            int32_t ret = deflateInit2(compress_stream_.get(), Z_BEST_COMPRESSION,
                                       Z_DEFLATED, MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);

            if (ret != Z_OK) {
                compress_stream_.reset();
            }
        };

        /***
         * @brief 解压缩流重置，私有
         *      1、 与ResetStream逻辑一致，但是需要调用inflateInit2初始化解压缩流
         *      2、 窗口大小同样设定为MAX_WBITS(需要与压缩式保持一致）
         */
        void ZlibCompress::ResetUncompressStream_() {
            uncompress_stream_ = std::unique_ptr<z_stream, ZStreamInflateDeleter>(new z_stream());
            uncompress_stream_->zalloc = Z_NULL;
            uncompress_stream_->zfree = Z_NULL;
            uncompress_stream_->opaque = Z_NULL;

            /// 初始化解压缩流
            int32_t ret = inflateInit2(uncompress_stream_.get(), MAX_WBITS);

            if (ret != Z_OK) {
                uncompress_stream_.reset();
            }
        }
    }
}