/**
 * @file      log_throughtput_test.cpp
 * @brief     [简要说明此头文件的作用]
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
namespace fs = std::filesystem;

// 生成模拟的普通日志内容（包含时间、级别、消息）
string generate_log_message(size_t index) {
    static const vector<string> levels = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    static const vector<string> messages = {
            "User login successful",
            "Connection established",
            "Resource usage high",
            "Failed to read config",
            "Task completed",
            "Network timeout occurred",
            "Data validation passed",
            "Cache miss detected"
    };

    // 随机选择级别和消息，模拟真实日志多样性
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> level_dist(0, levels.size() - 1);
    uniform_int_distribution<> msg_dist(0, messages.size() - 1);

    // 生成带索引的日志，方便后续验证
    char log[256];
    snprintf(log, sizeof(log),
             "[2024-07-23 12:34:56.%03d] [%s] %s (id=%zu)",
             rand() % 1000,
             levels[level_dist(gen)].c_str(),
             messages[msg_dist(gen)].c_str(),
             index);
    return string(log);
}

// 测试用例类
class MMapLogThroughputTest : public testing::Test {
protected:
    MMapAux::fpath test_file = "log_throughput_test.dat";

    void SetUp() override {
        try {
            if (fs::exists(test_file)) {
                fs::remove(test_file);
            }
        } catch (...) {}
    }

    void TearDown() override {
        try {
            if (fs::exists(test_file)) {
                fs::remove(test_file);
            }
        } catch (...) {}
    }
};

// 测试指定时间内最多能写入多少条日志
TEST_F(MMapLogThroughputTest, MaxLogsPerSecond) {
    cout << "\n===== 开始测试每秒最大日志写入量 =====" << endl;

    MMapAux mmap(test_file);
    if (!mmap.IsValid_()) {
        FAIL() << "内存映射初始化失败，无法进行测试";
    }

    // 测试参数
    const seconds test_duration(1);  // 测试持续时间：1秒
    size_t log_count = 0;            // 日志计数器
    vector<string> logs;             // 存储生成的日志，用于验证
    size_t total_expected_size = 0;  // 预期总数据大小（累计所有日志长度）

    // 记录开始时间
    auto start_time = high_resolution_clock::now();

    // 在指定时间内尽可能多地写入日志
    while (duration_cast<seconds>(high_resolution_clock::now() - start_time) < test_duration) {
        // 生成一条普通日志
        string log = generate_log_message(log_count);
        logs.push_back(log);
        total_expected_size += log.size();

        // 写入内存映射
        mmap.Push(log.data(), log.size());
        log_count++;

        // 每写入10000条日志打印一次进度（避免IO影响测试）
        if (log_count % 10000 == 0) {
            auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count();
            cout << "已写入 " << log_count << " 条日志，耗时 " << elapsed << " ms" << endl;
        }
    }

    // 计算实际测试时间和吞吐量
    auto end_time = high_resolution_clock::now();
    auto elapsed_ms = duration_cast<milliseconds>(end_time - start_time).count();
    double elapsed_sec = elapsed_ms / 1000.0;
    double logs_per_sec = log_count / elapsed_sec;

    // 统计日志大小信息
    double avg_log_size = total_expected_size / (double)log_count;

    // 输出测试结果
    cout << "\n===== 测试结果 =====" << endl;
    cout << "测试持续时间: " << elapsed_ms << " ms" << endl;
    cout << "总写入日志条数: " << log_count << endl;
    cout << "平均日志大小: " << avg_log_size << " 字节" << endl;
    cout << "每秒最大日志写入量: " << logs_per_sec << " 条/秒" << endl;
    cout << "预期总数据量: " << total_expected_size << " 字节" << endl;
    cout << "实际映射数据量: " << mmap.Size() << " 字节" << endl;
    cout << "数据写入速率: " << (total_expected_size / (1024.0 * 1024)) / elapsed_sec << " MB/秒" << endl;

    // 验证总数据量是否匹配
    ASSERT_EQ(mmap.Size(), total_expected_size) << "总数据量不匹配，可能存在写入丢失";

    // 验证数据完整性（抽样验证前100条和后100条）
    const uint8_t* data = mmap.Data();
    size_t offset = 0;
    size_t verify_count = min((size_t)100, log_count);

    // 验证前100条
    for (size_t i = 0; i < verify_count; i++) {
        const string& expected = logs[i];
        // 修复判断逻辑：确保偏移量+当前日志长度不超过总数据量
        ASSERT_LE(offset + expected.size(), mmap.Size()) << "日志数据越界（前" << verify_count << "条）";
        ASSERT_EQ(memcmp(data + offset, expected.data(), expected.size()), 0)
                                    << "日志内容不匹配（第" << i << "条）";
        offset += expected.size();
    }

    // 验证后100条
    if (log_count > verify_count) {
        offset = 0;
        // 计算最后100条日志的起始偏移量
        for (size_t i = 0; i < log_count - verify_count; i++) {
            offset += logs[i].size();
        }

        for (size_t i = log_count - verify_count; i < log_count; i++) {
            const string& expected = logs[i];
            ASSERT_LE(offset + expected.size(), mmap.Size()) << "日志数据越界（后" << verify_count << "条）";
            ASSERT_EQ(memcmp(data + offset, expected.data(), expected.size()), 0)
                                        << "日志内容不匹配（第" << i << "条）";
            offset += expected.size();
        }
    }

    cout << "===== 测试完成 =====" << endl;
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
