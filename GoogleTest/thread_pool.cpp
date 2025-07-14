/**
 * @file      thread_pool.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include "context/thread_pool.h"
#include <atomic>
#include <chrono>
#include <future>
#include <vector>
#include <iostream>  // 用于打印

using namespace logger::ctx;
using namespace std::chrono_literals;

// 测试1：普通任务提交与执行
TEST(ThreadPoolTest, SubmitTask
) {
std::cout << "\n=== 测试普通任务提交与执行 ===" <<
std::endl;
ThreadPool pool(2);
ASSERT_TRUE(pool
.

Start()

) << "线程池启动失败";
std::cout << "线程池启动成功，开始提交任务..." <<
std::endl;

std::atomic<int> count(0);
// 提交10个任务，每个任务计数+1
for (
int i = 0;
i < 10; ++i) {
pool.SubmitTask([&count]() {
count++;
});
}

// 等待任务执行完成
std::this_thread::sleep_for(100ms);
ASSERT_EQ(count
.

load(),

10) << "任务执行数量不符";
std::cout << "普通任务测试通过：所有任务均执行完毕" <<
std::endl;

pool.

Stop();

}

// 测试2：带返回值的任务
TEST(ThreadPoolTest, SubmitRetTask
) {
std::cout << "\n=== 测试带返回值的任务 ===" <<
std::endl;
ThreadPool pool(1);
ASSERT_TRUE(pool
.

Start()

) << "线程池启动失败";
std::cout << "线程池启动成功，提交带返回值的任务..." <<
std::endl;

// 提交带返回值的任务（计算1+2）
auto future = pool.SubmitRetTask([](int a, int b) { return a + b; }, 1, 2);
int result = future.get();
ASSERT_EQ(result,
3) << "返回值计算错误";
std::cout << "带返回值任务测试通过：结果为 " << result <<
std::endl;

pool.

Stop();

}

// 测试3：并发提交任务
TEST(ThreadPoolTest, ConcurrentSubmit
) {
std::cout << "\n=== 测试并发提交任务 ===" <<
std::endl;
ThreadPool pool(4);
ASSERT_TRUE(pool
.

Start()

) << "线程池启动失败";
std::cout << "线程池启动成功，开始并发提交任务..." <<
std::endl;

std::atomic<int> sum(0);
const int kTaskNum = 1000;
std::vector<std::thread> submitters;

// 5个线程并发提交任务
for (
int i = 0;
i < 5; ++i) {
submitters.emplace_back([&pool, &sum]() {
for (
int j = 0;
j < 200; ++j) {
pool.SubmitTask([&sum]() {
sum++;
});
}
});
}

// 等待所有提交线程完成
for (
auto &t
: submitters) {
t.

join();

}

// 等待任务执行完成
std::this_thread::sleep_for(200ms);
ASSERT_EQ(sum
.

load(), kTaskNum

) << "并发任务计数错误";
std::cout << "并发提交任务测试通过：总执行任务数 " << sum.

load()

<< "/" << kTaskNum <<
std::endl;

pool.

Stop();

}

// 测试4：线程池停止后无法提交任务
TEST(ThreadPoolTest, SubmitAfterStop
) {
std::cout << "\n=== 测试线程池停止后提交任务 ===" <<
std::endl;
ThreadPool pool(1);
ASSERT_TRUE(pool
.

Start()

) << "线程池启动失败";
pool.

Stop();

std::cout << "线程池已停止，尝试提交任务..." <<
std::endl;

// 停止后提交任务应失败
std::atomic<bool> executed(false);
bool submit_ok = pool.SubmitTask([&executed]() {
    executed = true;
});
ASSERT_FALSE(submit_ok)
<< "停止后任务提交成功，不符合预期";
ASSERT_FALSE(executed
.

load()

) << "停止后任务被执行，不符合预期";
std::cout << "线程池停止后提交任务测试通过：任务提交失败且未执行" <<
std::endl;
}

// 测试5：任务执行异常（验证线程池稳定性）
TEST(ThreadPoolTest, TaskWithException
) {
std::cout << "\n=== 测试任务执行异常 ===" <<
std::endl;
ThreadPool pool(1);
ASSERT_TRUE(pool
.

Start()

) << "线程池启动失败";
std::cout << "线程池启动成功，提交含异常的任务..." <<
std::endl;

// 提交一个会抛出异常的任务
auto future = pool.SubmitRetTask([]() {
    throw std::runtime_error("test exception");
});

// 验证异常被捕获
bool exception_caught = false;
try {
future.

get();

} catch (
const std::runtime_error &e
) {
exception_caught = true;
std::cout << "捕获到预期异常：" << e.

what()

<<
std::endl;
}
ASSERT_TRUE(exception_caught)
<< "未捕获到预期异常";

// 验证线程池仍可用
auto future2 = pool.SubmitRetTask([]() { return 42; });
ASSERT_EQ(future2
.

get(),

42) << "异常后线程池不可用";
std::cout << "任务执行异常测试通过：异常被捕获且线程池正常工作" <<
std::endl;

pool.

Stop();

}

// 测试6：线程池未启动时提交任务
TEST(ThreadPoolTest, SubmitBeforeStart
) {
std::cout << "\n=== 测试线程池未启动时提交任务 ===" <<
std::endl;
ThreadPool pool(1);  // 未调用Start()
std::cout << "线程池未启动，尝试提交任务..." <<
std::endl;

std::atomic<bool> executed(false);
bool submit_ok = pool.SubmitTask([&executed]() {
    executed = true;
});

ASSERT_FALSE(submit_ok)
<< "未启动时任务提交成功，不符合预期";
ASSERT_FALSE(executed
.

load()

) << "未启动时任务被执行，不符合预期";
std::cout << "线程池未启动时提交任务测试通过：任务提交失败且未执行" <<
std::endl;
}

// 主函数：执行所有测试并打印总结果
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    std::cout << "===== 开始执行线程池测试 =====" << std::endl;
    int ret = RUN_ALL_TESTS();
    if (ret == 0) {
        std::cout << "\n===== 所有测试通过 =====" << std::endl;
    } else {
        std::cout << "\n===== 部分测试失败 =====" << std::endl;
    }
    return ret;
}
