/**
 * @file      thread_pool.cpp
 * @brief     [线程池googletest]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include "context/thread_pool.h"
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
TEST(ThreadPoolTest, SubmitTask_BasicExecution) {
    std::cout << "\n=== 测试基本任务提交与执行 ===" << std::endl;
    ThreadPool pool(2);  // 创建2个线程的线程池
    ASSERT_TRUE(pool.Start()) << "线程池启动失败";
    std::cout << "线程池启动成功，提交单个任务..." << std::endl;

    std::atomic<bool> task_executed(false);
    // 提交任务：修改原子变量状态
    pool.SubmitTask([&task_executed]() {
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

    pool.Stop();
}

// 测试2：多任务串行执行（单线程线程池）
TEST(ThreadPoolTest, SubmitTask_SerialExecution) {
    std::cout << "\n=== 测试多任务串行执行 ===" << std::endl;
    ThreadPool pool(1);  // 单线程线程池（任务应串行执行）
    ASSERT_TRUE(pool.Start()) << "线程池启动失败";
    std::cout << "单线程线程池启动，提交3个有序任务..." << std::endl;

    std::atomic<int> step(0);
    std::promise<void> p1, p2, p3;
    auto f1 = p1.get_future();
    auto f2 = p2.get_future();
    auto f3 = p3.get_future();

    // 任务1：step应为0，执行后改为1
    pool.SubmitTask([&step, &p1]() {
        EXPECT_EQ(step.load(), 0) << "任务1执行顺序错误";
        step = 1;
        std::cout << "任务1执行完成：step=1" << std::endl;
        p1.set_value();
    });
    // 任务2：step应为1，执行后改为2
    pool.SubmitTask([&step, &p2, &f1]() {
        f1.wait();
        EXPECT_EQ(step.load(), 1) << "任务2执行顺序错误";
        step = 2;
        std::cout << "任务2执行完成：step=2" << std::endl;
        p2.set_value();
    });
    // 任务3：step应为2，执行后改为3
    pool.SubmitTask([&step, &p3, &f2]() {
        f2.wait();
        EXPECT_EQ(step.load(), 2) << "任务3执行顺序错误";
        step = 3;
        std::cout << "任务3执行完成：step=3" << std::endl;
        p3.set_value();
    });

    // 等待所有任务执行
    f3.wait();

    ASSERT_EQ(step.load(), 3) << "多任务串行执行失败";
    std::cout << "多任务串行测试通过：所有任务按顺序执行" << std::endl;

    pool.Stop();
}

// 测试3：并发提交大量任务（验证线程池负载能力）
TEST(ThreadPoolTest, SubmitTask_ConcurrentSubmit) {
    std::cout << "\n=== 测试并发提交大量任务 ===" << std::endl;
    const int THREAD_COUNT = 4;
    const int TASK_COUNT = 10000;  // 提交10000个任务
    ThreadPool pool(THREAD_COUNT);
    ASSERT_TRUE(pool.Start()) << "线程池启动失败";
    std::cout << "线程池（" << THREAD_COUNT << "线程）启动，开始并发提交任务..." << std::endl;

    std::atomic<int> completed_tasks(0);
    std::vector<std::thread> submitters;

    // 启动5个线程并发提交任务
    for (int i = 0; i < 5; ++i) {
        submitters.emplace_back([&pool, &completed_tasks, i]() {
            int tasks_per_thread = TASK_COUNT / 5;
            for (int j = 0; j < tasks_per_thread; ++j) {
                pool.SubmitTask([&completed_tasks]() {
                    completed_tasks++;  // 每个任务完成后计数+1
                });
            }
            std::cout << "提交线程" << i << "完成：提交了" << tasks_per_thread << "个任务" << std::endl;
        });
    }

    // 等待所有提交线程完成
    for (auto& t : submitters) {
        t.join();
    }
    std::cout << "所有任务提交完成，等待执行..." << std::endl;

    // 等待所有任务执行完成（最多等待5秒）
    auto start = std::chrono::steady_clock::now();
    while (completed_tasks.load() != TASK_COUNT &&
           std::chrono::steady_clock::now() - start < 5s) {
        std::this_thread::sleep_for(10ms);  // 短暂休眠，减少CPU占用
    }

    // 验证总任务数
    ASSERT_EQ(completed_tasks.load(), TASK_COUNT)
                                << "任务执行不完整：预期" << TASK_COUNT << "，实际" << completed_tasks.load();
    std::cout << "并发任务测试通过：所有" << TASK_COUNT << "个任务均执行完成" << std::endl;

    pool.Stop();
}

// 测试4：带参数的任务提交（验证std::bind参数转发）
TEST(ThreadPoolTest, SubmitTask_WithParameters) {
    std::cout << "\n=== 测试带参数的任务提交 ===" << std::endl;
    ThreadPool pool(1);
    ASSERT_TRUE(pool.Start()) << "线程池启动失败";
    std::cout << "线程池启动，提交带参数的任务..." << std::endl;

    // 测试参数：整数、字符串、引用
    int a = 10;
    std::string b = "hello";
    std::atomic<int> sum(0);
    std::promise<void> p;
    auto f = p.get_future();

    // 提交带参数的任务：计算a + b.length()，存入sum
    pool.SubmitTask([&sum, &p](int x, const std::string& y) {
        sum = x + y.length();
        std::cout << "带参数任务执行：x=" << x << ", y=" << y << ", 结果sum=" << sum << std::endl;
        p.set_value();
    }, a, b);  // 传递参数a和b

    // 等待任务执行
    f.wait();

    // 验证参数传递正确（10 + 5 = 15）
    ASSERT_EQ(sum.load(), 15) << "参数传递错误，任务执行结果不符";
    std::cout << "带参数任务测试通过：参数转发正确，结果符合预期" << std::endl;

    pool.Stop();
}

// 测试5：线程池状态对任务提交的影响（停止/未启动时提交）
TEST(ThreadPoolTest, SubmitTask_PoolState) {
    std::cout << "\n=== 测试线程池状态对任务的影响 ===" << std::endl;

    // 测试1：未启动的线程池
    ThreadPool pool1(1);  // 未调用Start()
    std::atomic<bool> task1_executed(false);
    pool1.SubmitTask([&task1_executed]() { task1_executed = true; });
    ASSERT_FALSE(task1_executed.load()) << "未启动的线程池不应执行任务";
    std::cout << "未启动线程池测试：任务未执行（符合预期）" << std::endl;

    // 测试2：已停止的线程池
    ThreadPool pool2(1);
    pool2.Start();
    pool2.Stop();  // 先启动再停止
    std::atomic<bool> task2_executed(false);
    pool2.SubmitTask([&task2_executed]() { task2_executed = true; });
    ASSERT_FALSE(task2_executed.load()) << "已停止的线程池不应执行任务";
    std::cout << "已停止线程池测试：任务未执行（符合预期）" << std::endl;

    std::cout << "线程池状态测试通过" << std::endl;
}

// 主函数：执行所有测试
//int main(int argc, char **argv) {
//    testing::InitGoogleTest(&argc, argv);
//    std::cout << "===== 开始执行线程池 SubmitTask 测试 =====" << std::endl;
//    int ret = RUN_ALL_TESTS();
//    if (ret == 0) {
//        std::cout << "\n===== 所有测试通过 =====" << std::endl;
//    } else {
//        std::cout << "\n===== 部分测试失败 =====" << std::endl;
//    }
//    return ret;
//}