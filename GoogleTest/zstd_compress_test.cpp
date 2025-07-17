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

using namespace logger::compress;

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

//int main(int argc, char **argv) {
//    std::cout << "初始化GoogleTest框架..." << std::endl;
//    ::testing::InitGoogleTest(&argc, argv);
//    std::cout << "开始执行ZstdCompress测试用例...\n" << std::endl;
//    int result = RUN_ALL_TESTS();
//    std::cout << "\n所有测试执行完毕，返回码: " << result << std::endl;
//    return result;
//}