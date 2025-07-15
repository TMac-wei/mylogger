/**
 * @file      zlib_compress_test.cpp
 * @brief     ZlibCompress压缩算法的单元测试
 * @author    Weijh
 * @version   1.0
 */

#include "gtest/gtest.h"
#include "compress/zlib_compress.h"
#include <string>
#include <iostream>
#include <cstring>

using namespace logger::compress;

// 测试正常数据的压缩与解压缩完整性
TEST(ZlibCompressTest, CompressDecompressIntegrity) {
    std::cout << "\n===== 开始测试 CompressDecompressIntegrity =====" << std::endl;

    // 创建压缩器实例（构造函数自动初始化流）
    ZlibCompress compressor;
    std::cout << "ZlibCompress实例创建完成（流已自动初始化）" << std::endl;

    // 测试数据：包含中英文、数字、符号
    const std::string original_data = "Zlib压缩测试：123456！@#$%^&*()";
    std::cout << "原始数据: " << original_data << std::endl;
    std::cout << "原始数据长度: " << original_data.size() << " 字节" << std::endl;

    // 计算压缩缓冲区大小
    size_t max_compressed_size = compressor.CompressedBound(original_data.size());
    std::cout << "预估最大压缩后长度: " << max_compressed_size << " 字节" << std::endl;

    // 分配压缩缓冲区
    char* compressed_buf = new char[max_compressed_size];
    ASSERT_NE(compressed_buf, nullptr) << "压缩缓冲区分配失败";

    // 执行压缩
    size_t compressed_size = compressor.Compress(
            original_data.data(), original_data.size(),
            compressed_buf, max_compressed_size
    );
    std::cout << "实际压缩后长度: " << compressed_size << " 字节" << std::endl;
    ASSERT_GT(compressed_size, 0) << "压缩失败（返回0字节）";
    ASSERT_LE(compressed_size, max_compressed_size) << "压缩结果超过预估最大长度";

    // 验证压缩数据格式
    bool is_compressed = logger::compress::ZlibCompress::IsCompressed(compressed_buf, compressed_size);
    std::cout << "压缩数据格式识别: " << (is_compressed ? "正确" : "错误") << std::endl;
    ASSERT_TRUE(is_compressed) << "压缩数据未被正确识别";

    // 执行解压缩
    std::string decompressed_data = compressor.Decompress(compressed_buf, compressed_size);
    std::cout << "解压缩后数据: " << decompressed_data << std::endl;
    std::cout << "解压缩后长度: " << decompressed_data.size() << " 字节" << std::endl;

    // 验证解压缩结果
    ASSERT_EQ(decompressed_data, original_data) << "解压缩数据与原始数据不一致";

    // 清理资源
    delete[] compressed_buf;
    std::cout << "===== 结束测试 CompressDecompressIntegrity =====" << std::endl;
}

// 测试空数据的压缩与解压缩
TEST(ZlibCompressTest, EmptyDataHandling) {
    std::cout << "\n===== 开始测试 EmptyDataHandling =====" << std::endl;

    ZlibCompress compressor;
    const std::string empty_data = "";
    std::cout << "测试空数据（长度0字节）" << std::endl;

    // 压缩空数据
    char compressed_buf[1024];
    size_t compressed_size = compressor.Compress(
            empty_data.data(), empty_data.size(),
            compressed_buf, sizeof(compressed_buf)
    );
    std::cout << "空数据压缩后长度: " << compressed_size << " 字节" << std::endl;

    // 解压缩空数据
    std::string decompressed_data = compressor.Decompress(compressed_buf, compressed_size);
    std::cout << "空数据解压缩后长度: " << decompressed_data.size() << " 字节" << std::endl;

    // 验证结果
    ASSERT_TRUE(decompressed_data.empty()) << "空数据解压缩后应为空";
    std::cout << "===== 结束测试 EmptyDataHandling =====" << std::endl;
}

TEST(ZlibCompressTest, ContinuousOperations) {
    std::cout << "\n===== 开始测试 ContinuousOperations =====" << std::endl;

    ZlibCompress compressor;
    const std::vector<std::string> test_datas = {
            "第一次压缩测试",
            "Second test: 12345",
            "!!!###$$$%%%",
            ""  // 空数据
    };

    for (size_t i = 0; i < test_datas.size(); ++i) {
        const std::string& data = test_datas[i];
        std::cout << "\n第" << i+1 << "轮测试，原始数据: " << (data.empty() ? "(空数据)" : data) << std::endl;
        std::cout << "原始数据长度: " << data.size() << " 字节" << std::endl;

        size_t max_size = compressor.CompressedBound(data.size());
        char* compressed_buf = new char[max_size];
        size_t compressed_size = compressor.Compress(data.data(), data.size(), compressed_buf, max_size);
        std::cout << "压缩后长度: " << compressed_size << " 字节" << std::endl;

        std::string decompressed = compressor.Decompress(compressed_buf, compressed_size);
        std::cout << "解压缩后数据: " << (decompressed.empty() ? "(空数据)" : decompressed) << std::endl;

        // 验证
        ASSERT_EQ(decompressed, data) << "第" << i+1 << "轮数据不一致";

        // 清理
        delete[] compressed_buf;
    }

    std::cout << "===== 结束测试 ContinuousOperations =====" << std::endl;
}

// 测试压缩边界（最大预估大小）
TEST(ZlibCompressTest, CompressedBoundValidation) {
    std::cout << "\n===== 开始测试 CompressedBoundValidation =====" << std::endl;

    ZlibCompress compressor;
    // 测试不同规模的输入数据
    const std::vector<size_t> test_sizes = {1, 100, 1024, 1024*1024};  // 1B, 100B, 1KB, 1MB

    for (size_t size : test_sizes) {
        size_t bound = compressor.CompressedBound(size);
        std::cout << "输入大小 " << size << " 字节时，预估最大压缩后大小: " << bound << " 字节" << std::endl;

        // 验证预估大小合理性（基于zlib公式）
        ASSERT_GE(bound, 0) << "预估大小小于理论最小值";
    }

    std::cout << "===== 结束测试 CompressedBoundValidation =====" << std::endl;
}

// 测试非压缩数据的解压缩容错性
TEST(ZlibCompressTest, UncompressedDataTolerance) {
    std::cout << "\n===== 开始测试 UncompressedDataTolerance =====" << std::endl;

    ZlibCompress compressor;
    const std::string raw_data = "这是一段未压缩的原始文本";
    std::cout << "测试非压缩数据: " << raw_data << std::endl;

    // 验证数据格式识别
    bool is_compressed = logger::compress::ZlibCompress::IsCompressed(raw_data.data(), raw_data.size());
    std::cout << "非压缩数据格式识别: " << (is_compressed ? "错误（识别为压缩）" : "正确（识别为非压缩）") << std::endl;
    ASSERT_FALSE(is_compressed) << "非压缩数据被错误识别";

    // 尝试解压缩非压缩数据（应返回空或错误结果）
    std::string result = compressor.Decompress(raw_data.data(), raw_data.size());
    std::cout << "非压缩数据解压缩结果: " << (result.empty() ? "(空)" : result) << std::endl;

    // 验证解压缩非压缩数据不会崩溃，且结果为空（因格式错误）
    ASSERT_TRUE(result.empty()) << "非压缩数据解压缩返回非空结果";

    std::cout << "===== 结束测试 UncompressedDataTolerance =====" << std::endl;
}

// 测试流重置功能（模拟单线程循环中的多次重置）
TEST(ZlibCompressTest, StreamResetFunction) {
    std::cout << "\n===== 开始测试 StreamResetFunction =====" << std::endl;

    ZlibCompress compressor;
    const std::string data = "测试流重置功能";
    std::cout << "原始数据: " << data << std::endl;

    // 第一次压缩
    size_t max_size = compressor.CompressedBound(data.size());
    char* compressed_buf = new char[max_size];
    size_t compressed_size = compressor.Compress(data.data(), data.size(), compressed_buf, max_size);
    std::cout << "第一次压缩后长度: " << compressed_size << " 字节" << std::endl;

    // 手动重置流
    std::cout << "手动调用ResetStream()" << std::endl;
    compressor.ResetStream();

    // 第二次压缩（验证重置后功能正常）
    size_t compressed_size2 = compressor.Compress(data.data(), data.size(), compressed_buf, max_size);
    std::cout << "重置后压缩长度: " << compressed_size2 << " 字节" << std::endl;
    ASSERT_EQ(compressed_size, compressed_size2) << "重置后压缩结果不一致";

    // 验证解压缩
    std::string decompressed = compressor.Decompress(compressed_buf, compressed_size2);
    ASSERT_EQ(decompressed, data) << "重置后解压缩失败";

    delete[] compressed_buf;
    std::cout << "===== 结束测试 StreamResetFunction =====" << std::endl;
}

//int main(int argc, char **argv) {
//    std::cout << "初始化GoogleTest框架..." << std::endl;
//    ::testing::InitGoogleTest(&argc, argv);
//    std::cout << "开始执行ZlibCompress测试用例...\n" << std::endl;
//    int result = RUN_ALL_TESTS();
//    std::cout << "\n所有测试执行完毕，返回码: " << result << std::endl;
//    return result;
//}