/**
 * @file      zstd_compress_test.cpp
 * @brief     ZstdCompress 压缩算法的单元测试
 * @author    Weijh
 * @version   1.0
 */

#include "gtest/gtest.h"
#include "compress/zstd_compress.h"
#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <random>
#include <filesystem>
#include <chrono>

using namespace logger::compress;
namespace fs = std::filesystem;


// 生成临时大文件的工具函数（测试完成后自动删除）
std::string create_temp_large_file(size_t size, bool random_data = true) {
    // 创建临时文件路径
    static std::random_device rd;
    std::string filename = "temp_test_" + std::to_string(rd()) + ".dat";
    fs::path temp_path = fs::temp_directory_path() / filename;

    std::ofstream file(temp_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建临时文件: " + temp_path.string());
    }

    // 生成数据（随机数据/重复数据）
    const size_t block_size = 1024 * 1024; // 1MB块
    std::vector<char> block(block_size);

    if (random_data) {
        // 随机数据（压缩率低）
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        for (char& c : block) {
            c = static_cast<char>(dist(gen));
        }
    } else {
        // 重复数据（压缩率高，填充固定字符）
        std::memset(block.data(), 'A', block_size);
    }

    // 写入文件
    size_t remaining = size;
    while (remaining > 0) {
        size_t write_size = std::min(remaining, block_size);
        file.write(block.data(), write_size);
        remaining -= write_size;
    }

    return temp_path.string();
}

// 读取文件内容到内存
std::vector<char> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("文件读取失败: " + path);
    }

    return buffer;
}


// 测试基本压缩和解压缩功能
TEST(ZstdCompressTest, BasicCompressDecompress) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.BasicCompressDecompress =====" << std::endl;

    ZstdCompress compressor;
    const std::string original = "ZSTD压缩测试：包含中英文、数字123和符号!@#$%";
    std::cout << "原始数据: " << original << std::endl;
    std::cout << "原始数据长度: " << original.size() << " 字节" << std::endl;

    // 计算压缩缓冲区大小
    size_t max_compressed_size = compressor.CompressedBound(original.size());
    std::cout << "预估最大压缩后长度: " << max_compressed_size << " 字节" << std::endl;

    // 分配压缩缓冲区
    std::vector<char> compressed_buf(max_compressed_size);
    size_t compressed_size = compressor.Compress(
            original.data(), original.size(),
            compressed_buf.data(), compressed_buf.size()
    );
    std::cout << "实际压缩后长度: " << compressed_size << " 字节" << std::endl;
    ASSERT_GT(compressed_size, 0) << "压缩失败（返回0字节）";
    ASSERT_LE(compressed_size, max_compressed_size) << "压缩后长度超过预估最大值";

    // 验证压缩格式
    bool is_compressed = ZstdCompress::IsCompressed(compressed_buf.data(), compressed_size);
    std::cout << "压缩数据格式识别: " << (is_compressed ? "正确" : "错误") << std::endl;
    ASSERT_TRUE(is_compressed) << "压缩数据未被正确识别";

    // 解压缩
    std::string decompressed = compressor.Decompress(compressed_buf.data(), compressed_size);
    std::cout << "解压缩后数据: " << decompressed << std::endl;
    std::cout << "解压缩后长度: " << decompressed.size() << " 字节" << std::endl;

    // 验证结果一致性
    ASSERT_EQ(decompressed, original) << "解压缩数据与原始数据不一致";

    std::cout << "===== 结束测试 ZstdCompressTest.BasicCompressDecompress =====" << std::endl;
}

// 测试空数据压缩和解压缩
TEST(ZstdCompressTest, EmptyData) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.EmptyData =====" << std::endl;

    ZstdCompress compressor;
    const std::string original = "";  // 空数据
    std::cout << "测试空数据压缩（长度0字节）" << std::endl;

    // 压缩空数据
    char compressed_buf[1024];
    size_t compressed_size = compressor.Compress(
            original.data(), original.size(),
            compressed_buf, sizeof(compressed_buf)
    );
    std::cout << "空数据压缩后长度: " << compressed_size << " 字节" << std::endl;

    // 解压缩空数据
    std::string decompressed = compressor.Decompress(compressed_buf, compressed_size);
    std::cout << "空数据解压缩后长度: " << decompressed.size() << " 字节" << std::endl;

    // 验证结果
    ASSERT_TRUE(decompressed.empty()) << "空数据解压缩后不为空";

    std::cout << "===== 结束测试 ZstdCompressTest.EmptyData =====" << std::endl;
}

// 测试大数据压缩（验证动态缓冲区）
TEST(ZstdCompressTest, LargeData) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.LargeData =====" << std::endl;

    ZstdCompress compressor;
    // 生成1MB的测试数据
    const size_t large_size = 1024 * 1024;  // 1MB
    std::string original(large_size, 'A');  // 填充'A'
    std::cout << "测试大数据压缩（长度 " << large_size << " 字节）" << std::endl;

    // 压缩
    size_t max_compressed_size = compressor.CompressedBound(large_size);
    std::vector<char> compressed_buf(max_compressed_size);
    size_t compressed_size = compressor.Compress(original.data(), large_size, compressed_buf.data(), max_compressed_size);
    std::cout << "大数据压缩后长度: " << compressed_size << " 字节" << std::endl;
    ASSERT_GT(compressed_size, 0) << "大数据压缩失败";

    // 解压缩（验证动态缓冲区是否能处理大尺寸）
    std::string decompressed = compressor.Decompress(compressed_buf.data(), compressed_size);
    std::cout << "大数据解压缩后长度: " << decompressed.size() << " 字节" << std::endl;

    // 验证结果
    ASSERT_EQ(decompressed.size(), large_size) << "大数据解压缩后长度不一致";
    ASSERT_EQ(decompressed, original) << "大数据解压缩内容不一致";

    std::cout << "===== 结束测试 ZstdCompressTest.LargeData =====" << std::endl;
}

// 测试连续多次压缩解压缩
TEST(ZstdCompressTest, ContinuousOperations) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.ContinuousOperations =====" << std::endl;

    ZstdCompress compressor;
    const std::vector<std::string> test_data = {
            "第一次压缩",
            "Second compression test: 456",
            "!!!$$$### 特殊符号测试 ###$$$!!!",
            ""  // 包含空数据
    };

    for (size_t i = 0; i < test_data.size(); ++i) {
        const std::string& data = test_data[i];
        std::cout << "\n第" << i+1 << "轮测试，原始数据: " << (data.empty() ? "(空数据)" : data) << std::endl;
        std::cout << "原始数据长度: " << data.size() << " 字节" << std::endl;

        /// 压缩
        size_t max_size = compressor.CompressedBound(data.size());
        std::vector<char> compressed_buf(max_size);
        size_t compressed_size = compressor.Compress(data.data(), data.size(), compressed_buf.data(), max_size);
        std::cout << "第" << i+1 << "轮压缩后长度: " << compressed_size << " 字节" << std::endl;

        // 修正断言逻辑：空数据允许返回0，非空数据必须>0
        if (data.empty()) {
            // 空数据压缩后可以是0字节（合理）
            ASSERT_EQ(compressed_size, 0) << "第" << i+1 << "轮空数据压缩长度错误";
        } else {
            // 非空数据压缩后必须>0
            ASSERT_GT(compressed_size, 0) << "第" << i+1 << "轮压缩失败";
        }

        // 解压缩
        std::string decompressed = compressor.Decompress(compressed_buf.data(), compressed_size);
        std::cout << "第" << i+1 << "轮解压缩后长度: " << decompressed.size() << " 字节" << std::endl;

        // 验证
        ASSERT_EQ(decompressed, data) << "第" << i+1 << "轮数据不一致";
    }

    std::cout << "===== 结束测试 ZstdCompressTest.ContinuousOperations =====" << std::endl;
}

// 测试非ZSTD数据的解压缩（容错性）
TEST(ZstdCompressTest, NonZstdData) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.NonZstdData =====" << std::endl;

    ZstdCompress compressor;
    const std::string raw_data = "这是一段未压缩的原始文本，不是ZSTD格式";
    std::cout << "测试非ZSTD数据解压缩: " << raw_data << std::endl;

    // 验证格式识别
    bool is_compressed = ZstdCompress::IsCompressed(raw_data.data(), raw_data.size());
    std::cout << "非ZSTD数据格式识别: " << (is_compressed ? "错误（误判为压缩）" : "正确（识别为非压缩）") << std::endl;
    ASSERT_FALSE(is_compressed) << "非压缩数据被错误识别";

    // 尝试解压缩非ZSTD数据
    std::string result = compressor.Decompress(raw_data.data(), raw_data.size());
    std::cout << "非ZSTD数据解压缩结果: " << (result.empty() ? "空（正确）" : "非空（错误）") << std::endl;
    ASSERT_TRUE(result.empty()) << "非ZSTD数据解压缩返回非空结果";

    std::cout << "===== 结束测试 ZstdCompressTest.NonZstdData =====" << std::endl;
}

// 测试压缩边界预估
TEST(ZstdCompressTest, CompressedBound) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.CompressedBound =====" << std::endl;

    ZstdCompress compressor;
    const std::vector<size_t> test_sizes = {1, 100, 1024, 1024*1024};  // 1B, 100B, 1KB, 1MB

    for (size_t size : test_sizes) {
        size_t bound = compressor.CompressedBound(size);
        std::cout << "输入大小 " << size << " 字节时，预估最大压缩后大小: " << bound << " 字节" << std::endl;

        // 验证预估大小合理性（ZSTD_compressBound返回值一定 >= 输入大小）
        ASSERT_GE(bound, size) << "预估最大压缩大小小于输入大小（不合理）";
    }

    std::cout << "===== 结束测试 ZstdCompressTest.CompressedBound =====" << std::endl;
}


// 新增：大文件压缩率测试（针对不同类型数据）
TEST(ZstdCompressTest, LargeFileCompressionRatio) {
    std::cout << "\n===== 开始测试 ZstdCompressTest.LargeFileCompressionRatio =====" << std::endl;

    ZstdCompress compressor;
    // 测试不同大小的文件（10MB、50MB）
    const std::vector<size_t> file_sizes = {10 * 1024 * 1024, 50 * 1024 * 1024}; // 10MB, 50MB

    for (size_t file_size : file_sizes) {
        // 测试两种数据类型：随机数据（难压缩）、重复数据（易压缩）
        for (bool is_random : {true, false}) {
            try {
                std::cout << "\n测试" << (is_random ? "随机" : "重复") << "数据文件，大小: "
                          << file_size / (1024 * 1024) << "MB" << std::endl;

                // 创建临时大文件
                std::string file_path = create_temp_large_file(file_size, is_random);
                std::cout << "临时文件路径: " << file_path << std::endl;

                // 读取文件内容
                std::vector<char> file_data = read_file(file_path);
                ASSERT_EQ(file_data.size(), file_size) << "文件读取大小不一致";

                // 压缩并计时
                auto start = std::chrono::high_resolution_clock::now();
                size_t max_compressed_size = compressor.CompressedBound(file_size);
                std::vector<char> compressed_buf(max_compressed_size);
                size_t compressed_size = compressor.Compress(
                        file_data.data(), file_size,
                        compressed_buf.data(), max_compressed_size
                );
                auto end = std::chrono::high_resolution_clock::now();
                double compress_time = std::chrono::duration<double, std::milli>(end - start).count();

                // 验证压缩结果
                ASSERT_GT(compressed_size, 0) << "大文件压缩失败";
                ASSERT_LE(compressed_size, max_compressed_size) << "压缩后超出预估大小";

                // 计算压缩率和速度
                double compression_ratio = static_cast<double>(compressed_size) / file_size * 100;
                double compression_bi = static_cast<double>(file_size) / compressed_size;
                double compress_speed = (file_size / (1024 * 1024)) / (compress_time / 1000); // MB/s

                std::cout << "压缩性能指标:" << std::endl;
                std::cout << "  原始大小: " << file_size << " 字节" << std::endl;
                std::cout << "  压缩后大小: " << compressed_size << " 字节" << std::endl;
                std::cout << "  压缩率: " << compression_ratio << "%" << std::endl;
                std::cout << "  压缩比: " << compression_bi << "%" << std::endl;
                std::cout << "  压缩时间: " << compress_time << " ms" << std::endl;
                std::cout << "  压缩速度: " << compress_speed << " MB/s" << std::endl;

                // 解压缩并验证
                start = std::chrono::high_resolution_clock::now();
                std::string decompressed = compressor.Decompress(compressed_buf.data(), compressed_size);
                end = std::chrono::high_resolution_clock::now();
                double decompress_time = std::chrono::duration<double, std::milli>(end - start).count();
                double decompress_speed = (file_size / (1024 * 1024)) / (decompress_time / 1000); // MB/s

                std::cout << "  解压缩时间: " << decompress_time << " ms" << std::endl;
                std::cout << "  解压缩速度: " << decompress_speed << " MB/s" << std::endl;
                ASSERT_EQ(decompressed.size(), file_size) << "解压缩后大小不一致";
                ASSERT_EQ(memcmp(decompressed.data(), file_data.data(), file_size), 0) << "解压缩内容不一致";

                // 删除临时文件
                fs::remove(file_path);
            } catch (const std::exception& e) {
                FAIL() << "测试失败: " << e.what();
            }
        }
    }

    std::cout << "\n===== 结束测试 ZstdCompressTest.LargeFileCompressionRatio =====" << std::endl;
}

//int main(int argc, char **argv) {
//    std::cout << "初始化GoogleTest框架..." << std::endl;
//    ::testing::InitGoogleTest(&argc, argv);
//    std::cout << "开始执行ZstdCompress测试用例...\n" << std::endl;
//    int result = RUN_ALL_TESTS();
//    std::cout << "\n所有测试执行完毕，返回码: " << result << std::endl;
//    return result;
//}