/**
 * @file      strand_executor.cpp
 * @brief     [strand_executor谷歌测试]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include "context/executor.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <future>

using namespace logger::ctx;
using namespace std::chrono_literals;

// 测试1：基本任务提交与执行（验证任务是否被执行）
TEST(ExecutorTest, PostTask_BasicExecution) {
    std::cout << "\n=== 测试基本任务提交与执行 ===" << std::endl;
    Executor executor;
    TaskRunnerTag runnerTag = executor.AddTaskRunner(1);

    std::atomic<bool> task_executed(false);
    // 提交任务：修改原子变量状态
    executor.PostTask(runnerTag, [&task_executed]() {
        task_executed = true;
        std::cout << "任务执行中：标记任务为已执行" << std::endl;
    });

    // 等待任务执行（最多等待1秒，避免无限阻塞）
    auto start = std::chrono::steady_clock::now();
    while (!task_executed.load() &&
           std::chrono::steady_clock::now() - start < 1s) {
        std::this_thread::yield();  // 让出CPU时间片
    }

    // 验证任务执行结果
    ASSERT_TRUE(task_executed.load()) << "任务未被执行";
    std::cout << "基本任务测试通过：任务已正确执行" << std::endl;
}

// 测试2：延迟任务提交与执行
TEST(ExecutorTest, PostDelayedTask_Execution) {
    std::cout << "\n=== 测试延迟任务提交与执行 ===" << std::endl;
    Executor executor;
    TaskRunnerTag runnerTag = executor.AddTaskRunner(1);

    std::atomic<bool> task_executed(false);
    // 提交延迟任务：100ms后修改原子变量状态
    executor.PostDelaydTask(runnerTag, [&task_executed]() {
        task_executed = true;
        std::cout << "延迟任务执行中：标记任务为已执行" << std::endl;
    }, 100ms);

    // 等待任务执行（最多等待1秒，避免无限阻塞）
    auto start = std::chrono::steady_clock::now();
    while (!task_executed.load() &&
           std::chrono::steady_clock::now() - start < 1s) {
        std::this_thread::yield();  // 让出CPU时间片
    }

    // 验证任务执行结果
    ASSERT_TRUE(task_executed.load()) << "延迟任务未被执行";
    std::cout << "延迟任务测试通过：任务已正确执行" << std::endl;
}

// 测试3：周期性任务提交与执行
TEST(ExecutorTest, PostRepeatedTask_Execution) {
    std::cout << "\n=== 测试周期性任务提交与执行 ===" << std::endl;
    Executor executor;
    TaskRunnerTag runnerTag = executor.AddTaskRunner(1);

    std::atomic<int> task_count(0);
    // 提交周期性任务：100ms执行一次，执行3次
    RepeatedTaskId taskId = executor.PostRepeatedTask(runnerTag, [&task_count]() {
        task_count++;
        std::cout << "周期性任务执行中：任务计数=" << task_count << std::endl;
    }, 100ms, 3);

    // 等待任务执行（最多等待1秒，避免无限阻塞）
    auto start = std::chrono::steady_clock::now();
    while (task_count.load() < 3 &&
           std::chrono::steady_clock::now() - start < 1s) {
        std::this_thread::yield();  // 让出CPU时间片
    }

    // 验证任务执行结果
    ASSERT_EQ(task_count.load(), 3) << "周期性任务执行次数不符";
    std::cout << "周期性任务测试通过：任务已正确执行" << std::endl;

    // 取消任务
    executor.CancelRepeatedTask(taskId);
}

// 测试4：带返回值的任务提交与执行
TEST(ExecutorTest, PostTaskAndGetResult_Execution) {
    std::cout << "\n=== 测试带返回值的任务提交与执行 ===" << std::endl;
    Executor executor;
    TaskRunnerTag runnerTag = executor.AddTaskRunner(1);

    // 提交带返回值的任务：计算1 + 2
    auto future = executor.PostTaskAndGetResult(runnerTag, [](int a, int b) { return a + b; }, 1, 2);

    // 等待任务执行并获取结果
    auto result = future->get();

    // 验证任务执行结果
    ASSERT_EQ(result, 3) << "带返回值任务执行结果不符";
    std::cout << "带返回值任务测试通过：结果=" << result << std::endl;
}

