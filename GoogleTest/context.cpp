/**
 * @file      context.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "context/context.h"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace logger::ctx;

// 测试单例模式唯一性
TEST(ContextTest, SingletonUniqueness) {
    Context& ctx1 = Context::GetInstance();
    Context& ctx2 = Context::GetInstance();
    EXPECT_EQ(&ctx1, &ctx2) << "单例模式失败：两次获取的实例不同";
    std::cout << "单例模式测试通过：实例地址一致" << std::endl;
}

// 测试基本任务提交与执行
TEST(ContextTest, BasicTaskExecution) {
    // 创建任务执行器标签
    auto tag = NEW_TASK_RUNNER(12345);
    std::atomic<bool> task_executed(false);

    // 提交任务
    POST_TASK(tag, [&task_executed]() {
        task_executed = true;
        std::cout << "基本任务执行中" << std::endl;
    });

    // 等待任务完成
    WAIT_TASK_IDLE(tag);
    EXPECT_TRUE(task_executed) << "基本任务未执行";
    std::cout << "基本任务测试通过" << std::endl;
}

// 测试周期任务执行次数
TEST(ContextTest, RepeatedTaskExecution) {
    auto tag = NEW_TASK_RUNNER(45678);
    std::atomic<size_t> exec_count(0);
    const size_t expected_repeats = 3;  // 预期执行3次

    // 提交周期任务（每100ms执行一次，共3次）
    POST_REPEATED_TASK(
            tag,
            [&exec_count]() {
                exec_count++;
                std::cout << "周期任务执行，当前次数：" << exec_count << std::endl;
            },
            std::chrono::milliseconds(100),
            expected_repeats
    );

    // 等待所有周期任务完成（额外等待100ms确保最后一次执行）
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * expected_repeats + 100));
    WAIT_TASK_IDLE(tag);

    EXPECT_EQ(exec_count, expected_repeats) << "周期任务执行次数不符";
    std::cout << "周期任务测试通过" << std::endl;
}

// 测试多线程下任务调度安全性
TEST(ContextTest, MultiThreadTaskSafety) {
    auto tag = NEW_TASK_RUNNER(11111);
    std::atomic<int> shared_counter(0);
    const int thread_count = 5;
    const int tasks_per_thread = 1000;

    // 多线程提交任务，并发修改计数器
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([tag, &shared_counter, tasks_per_thread]() {
            for (int j = 0; j < tasks_per_thread; ++j) {
                POST_TASK(tag, [&shared_counter]() {
                    shared_counter++;
                });
            }
        });
    }

    // 等待所有线程提交任务
    for (auto& t : threads) {
        t.join();
    }

    // 等待所有任务执行完成
    WAIT_TASK_IDLE(tag);

    // 验证计数器结果（预期：5*1000=5000）
    EXPECT_EQ(shared_counter, thread_count * tasks_per_thread) << "多线程任务执行异常";
    std::cout << "多线程任务测试通过，计数器结果：" << shared_counter << std::endl;
}

//int main(int argc, char **argv) {
//    testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}