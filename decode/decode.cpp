/**
 * @file      decode.cpp
 * @brief     [读取加密压缩的日志文件，使用 ECDH 密钥交换生成共享密钥，然后用 AES 解密，再用 Zstd 解压，最终将日志按指定格式输出到文件]
 * @author    Weijh
 * @version   1.0
 */

#include "decode_formatter.h"
#include "mylogger/compress/zstd_compress.h"
#include "mylogger/crypt/AES_crypt.h"
#include "mylogger/sinks/effective_sink.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>
#include <vector>

using namespace logger;
using namespace logger::detail;

/// 解码器
std::unique_ptr<DecodeFormatter> decode_formatter_;
/// 解压缩
std::unique_ptr<logger::compress::Compression> decompress_;

/**
 * @brief 从文件读取二进制数据到内存
 * @param  file_path文件路径
*/
std::vector<char> ReadFile(const std::string &file_path) {
    /// 文件检测
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("ReadFile: file does not exist");
    }
    /// 获取文件大小
    auto size = std::filesystem::file_size(file_path);

    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("ReadFile: open file failed!");
    }

    std::vector<char> buffer(size);
    if (!ifs.read(buffer.data(), size)) {
        throw std::runtime_error("ReadFile: read failed");
    }

    return buffer;
}

/**
 * @brief 将数据追加到文件末尾
 * @param file_path 文件路径
 * @param data  追加的数据
 *  以二进制追加模式打开文件并且追加字符串数据到末尾
 */
void AppendDataToFile(const std::string &file_path, const std::string &data) {
    /// 确保文件路径存在，如果不存在则创建
    auto dir = std::filesystem::path(file_path).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        if (!std::filesystem::create_directories(dir)) {
            throw std::runtime_error("AppendDataToFile: create directory failed!");
        }
    }

    std::ofstream ofs(file_path, std::ios::binary | std::ios::app);
    if (!ofs) {
        throw std::runtime_error("AppendDataToFile: open file failed");
    }
    if (!ofs.write(data.data(), data.size())) {
        throw std::runtime_error("AppendDataToFile: write failed");
    }
}

/**
 * @brief 单个加密压缩的日志条目，依次进行解密、解压、解析和格式化，最终将结果追加到输出字符串
 * @param data  拿到的压缩加密后的日志数据
 * @param size  压缩加密后的日志大小
 * @param crypt 加密指针
 * @param output    输出缓冲区
 *  完整流程
 *      解密 -> 解压缩 -> 解析protobuf数据 -> 格式化输出
 */
void DecodeLogItemData(char *data, size_t size, crypt::Crypt *crypt, std::string &output) {
    std::string decrypt_data = crypt->Decrypt(data, size);
    if (decrypt_data.empty()) {
        /// 解密失败
        throw std::runtime_error("DecodeItemData: decryption failed");
    }

    std::string decompressed_data = decompress_->Decompress(decrypt_data.data(), decrypt_data.size());
    if (decompressed_data.empty()) {
        /// 解压缩失败
        throw std::runtime_error("DecodeItemData: decompression failed");
    }

    EffectiveMsg msg;
    if (!msg.ParseFromString(decompressed_data)) {
        /// 解析protobuf消息失败
        throw std::runtime_error("DecodeItemData: Protobuf parse failed");
    }

    /// 格式化输出
    std::string result;
    decode_formatter_->Format(msg, result);
    output.append(result);
}

/**
 * @brief 处理数据块，对日志文件中的一个数据块进行解析，生成解密所需要的密钥，循环处理每一条日志条目
 * @param data
 * @param size
 * @param client_pub_key    客户端公钥（需要其生成共享密钥）
 * @param ser_pri_key       传入的服务器私钥（十六进制）
 * @param output
 *  接收一个日志数据块的原始数据，通过 ECDH 密钥交换算法生成共享密钥，
 *  然后逐行解析数据块内的日志条目（每条条目包含头部和内容），最终将格式化后的日志内容追加到输出字符串中。
 */
void DecodeLogChunkData(char *data, size_t size, const std::string &client_pub_key,
                        const std::string &ser_pri_key, std::string &output) {
    std::cout << "decode chunk " << size << std::endl;

    /// 生成共享密钥
    std::string ser_pri_key_bin = crypt::HexKeyToBinary(ser_pri_key);
    std::string shared_key = crypt::GenerateECDHSharedKey(ser_pri_key_bin, client_pub_key);
    /// 创建Crypt，用于解密
    std::unique_ptr<crypt::Crypt> crypt1 = std::make_unique<crypt::AESCrypt>(shared_key);

    /// 循环解析块中的日志条目
    size_t offset = 0;
    size_t count = 0;
    output.reserve(output.size() + size * 2);

    while (offset < size) {
        ++count;
        if (count % 1000 == 0) {
            /// 输出解析进度
            std::cout << "decode item " << count << " (progress: " << (offset * 100 / size) << "%)" << std::endl;
        }

        ///itemData头部解析并校验
        if (offset + sizeof(LogItemHeader) > size) {
            throw std::runtime_error("DecodeLogChunkData: insufficient size for ItemHeader");
        }

        LogItemHeader *item_header = reinterpret_cast<LogItemHeader *>(data + offset);
        if (item_header->magic != LogItemHeader::kMagic) {
            /// 魔数校验不通过
            throw std::runtime_error("DecodeLogChunkData: invalid item magic");
        }

        /// 剩下数据的长度不合规
        if (offset + sizeof(LogItemHeader) + item_header->size > size) {
            throw std::runtime_error("DecodeLogChunkData: item size exceeds remaining data");
        }

        /// 处理条目内容
        offset += sizeof(LogItemHeader);
        DecodeLogItemData(data + offset, item_header->size, crypt1.get(), output);
        offset += item_header->size;
        output.push_back('\n');
    }

}

/**
 * @brief 解析的入口，负责协调文件级别的解码流程，包括读取输入文件、解析数据块结构、调用数据块解码函数，并将结果写入输出文件
 * @param input_file_path       输入日志文件路径
 * @param pri_key               服务器私钥
 * @param out_file_path         输出文件路径
 *  通过读取输入文件、校验文件格式、解析数据块（Chunk）、调用数据块解码逻辑（DecodeChunkData），最终将解码后的日志内容写入输出文件
 */
void DecodeLogFile(const std::string &input_file_path, const std::string &pri_key, const std::string &out_file_path) {
    auto file_input = ReadFile(input_file_path);
    size_t file_size = file_input.size();

    /// 初始化校验，文件大小至少是一个块头部大小
    if (file_size < sizeof(LogChunkHeader)) {
        throw std::runtime_error("DecodeLogFile: input file is too small");
    }

    /// 通过偏移量进行循环解析
    size_t offset = 0;
    std::string output;
    /// 预分配内存，省的频繁扩容
    output.reserve(1024 * 1024);

    while (offset < file_size) {
        /// 检验剩余空间是否包含块头部信息
        if (offset + sizeof(LogChunkHeader) > file_size) {
            throw std::runtime_error("DecodeLogFile: incomplete chunk header");
        }

        /// 魔数校验
        LogChunkHeader *chunk_header = reinterpret_cast<LogChunkHeader *>(file_input.data() + offset);
        if (chunk_header->magic != LogChunkHeader::kMagic) {
            throw std::runtime_error("DecodeLogFile: invalid chunk magic");
        }

        /// 校验数据块大小是否合理
        if (offset + sizeof(LogChunkHeader) + chunk_header->size > file_size) {
            throw std::runtime_error("DecodeLogFile: chunk size exceeds remaining file size");
        }

        /// 校验公钥长度是否为65字节（ECDH公钥标准长度）
        if (std::string(chunk_header->pub_key, 65).size() != 65) {
            throw std::runtime_error("DecodeLogFile: invalid client public key length");
        }

        /// 所有校验都通过，开始解析
        output.clear();
        offset += sizeof(LogChunkHeader);
        DecodeLogChunkData(file_input.data() + offset, chunk_header->size, std::string(chunk_header->pub_key, 65),
                           pri_key, output);

        /// 偏移到下一个块
        offset += chunk_header->size;
        AppendDataToFile(out_file_path, output);
    }
}

/**
 * @brief 整个日志解码程序的入口点，负责解析命令行参数、初始化核心组件，并调用文件解码逻辑
 * @param argc
 * @param argv
 * @return
 * 日志解码器，通过命令行接收加密日志文件路径、服务器私钥和输出文件路径，然后执行解码操作，将加密压缩的日志还原为可读文本格式。
 */
int main(int argc, char *argv[]) {

#if 1
    /// 命令行输入参数校验
    if (argc != 4) {
        std::cerr << "Usage: ./decode <file_path> <pri_key> <output_file>" << std::endl;
        return 1;
    }

    /// 分别获取输入文件路径、服务器私钥、输出文件路径
    std::string input_file_path = argv[1];
    std::string pri_key = argv[2];
    std::string output_file_path = argv[3];

    /// 验证获得的私钥是否符合要求--> 私钥为十六进制字符串
    if (!std::all_of(pri_key.begin(), pri_key.end(), [](char c) {
        return std::isxdigit(c);
    }) || pri_key.size() != 64) {
        std::cerr
                << "Error: Invalid private key format (must be hex string) or invalid private length (pri_key.size() != 64)"
                << std::endl;
        return 1;
    }
#else
    std::string input_file_path = "D:/logger/loggerdemo_20241023210917.log";
    std::string pri_key = "FAA5BBE9017C96BF641D19D0144661885E831B5DDF52539EF1AB4790C05E665E";
    std::string output_file_path = "D:/logger/d.txt";
#endif

    try {
        /// 初始化解码器
        decode_formatter_ = std::make_unique<DecodeFormatter>();
        decode_formatter_->SetPattern("[%l][%D:%S][%p:%t][%F:%f:%#]%v");

        /// 初始化解密器
        decompress_ = std::make_unique<logger::compress::ZstdCompress>();

        /// 开始解码
        std::cout << "Decoding file: " << input_file_path << std::endl;
        DecodeLogFile(input_file_path, pri_key, output_file_path);
        std::cout << "Decoding completed. Output saved to: " << output_file_path << std::endl;

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Decode failed: " << e.what() << std::endl;
        return 1;
    }

}