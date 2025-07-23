/**
 * @file      mmap_test.cpp
 * @brief     [mmap延迟测试]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include <random>
#include "mylogger/mmap/mmap_aux.h"

using namespace logger;
using namespace std;
using namespace chrono;

// 生成随机数据的工具函数
void GenerateRandomData(uint8_t* data, size_t size) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<uint8_t> dist(0, 255);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dist(gen);
    }
}

// 测试用例类
class MMapAuxTest : public testing::Test {
protected:
    MMapAux::fpath test_file = "mmap_test.dat";

    void SetUp() override {
        try {
            if (filesystem::exists(test_file)) {
                filesystem::remove(test_file);
            }
        } catch (...) {}
    }

    void TearDown() override {
        try {
            if (filesystem::exists(test_file)) {
                filesystem::remove(test_file);
            }
        } catch (...) {}
    }

    template<typename Func>
    double measureTimeMs(Func func) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        duration<double, milli> duration = end - start;
        return duration.count();
    }
};

// 测试构造函数延迟
TEST_F(MMapAuxTest, ConstructionLatency) {
    double time = measureTimeMs([&]() {
        MMapAux mmap(test_file);
    });
    cout << "文件映射创建耗时: " << time << " ms" << endl;
    EXPECT_GT(time, 0);
}

// 测试随机数据写入延迟（小数据）
TEST_F(MMapAuxTest, RandomWriteSmall) {
    MMapAux mmap(test_file);

    // 新增：检查映射是否有效
    cout << "映射是否有效: " << (mmap.IsValid_() ? "是" : "否") << endl;  // 注意：需要将IsValid_()改为public才能访问，或新增一个public接口暴露有效性

    const size_t data_size = 1024; // 1KB
    unique_ptr<uint8_t[]> data(new uint8_t[data_size]);
    GenerateRandomData(data.get(), data_size); // 生成随机数据

    double time = measureTimeMs([&]() {
        mmap.Push(data.get(), data_size);
    });

    cout << "写入" << data_size << "字节随机数据耗时: " << time << " ms" << endl;
    EXPECT_EQ(mmap.Size(), data_size);
}

// 测试随机数据写入延迟（大数据）
TEST_F(MMapAuxTest, RandomWriteLarge) {
    MMapAux mmap(test_file);
    const size_t data_size = 1024 * 1024; // 1MB
    unique_ptr<uint8_t[]> data(new uint8_t[data_size]);
    GenerateRandomData(data.get(), data_size); // 生成随机数据

    double time = measureTimeMs([&]() {
        mmap.Push(data.get(), data_size);
    });

    cout << "写入" << data_size << "字节随机数据耗时: " << time << " ms" << endl;
    EXPECT_EQ(mmap.Size(), data_size);
}

// 测试多次随机写入延迟
TEST_F(MMapAuxTest, MultipleRandomWrites) {
    MMapAux mmap(test_file);
    const size_t chunk_size = 512 * 1024; // 512KB
    const int iterations = 5;
    unique_ptr<uint8_t[]> data(new uint8_t[chunk_size]);

    double time = measureTimeMs([&]() {
        for (int i = 0; i < iterations; ++i) {
            GenerateRandomData(data.get(), chunk_size); // 每次生成不同的随机数据
            mmap.Push(data.get(), chunk_size);
        }
    });

    size_t total = chunk_size * iterations;
    cout << "累计写入" << total << "字节随机数据（分" << iterations << "次）耗时: "
         << time << " ms，平均每次: " << time / iterations << " ms" << endl;
    EXPECT_EQ(mmap.Size(), total);
}

//int main(int argc, char **argv) {
//    testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}