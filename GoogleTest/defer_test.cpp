/**
 * @file      defer_test.cpp
 * @brief     [defer_test] 信息打印版
 * @author    Weijh
 * @version   1.0
 */

#include "defer.h"

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <string>

// 测试基本功能
TEST(ExecuteOnScopeExitTest, BasicFunctionality) {
    std::cout << "[Test] BasicFunctionality" << std::endl;
    bool executed = false;

    {
        LOG_DEFER {
            std::cout << ">> defer executed inside scope (BasicFunctionality)" << std::endl;
            executed = true;
        };
        EXPECT_FALSE(executed);
    }

    EXPECT_TRUE(executed);
}

// 测试移动语义：确保资源正确转移（这个测试会触发死亡，因此不会继续执行）
TEST(ExecuteOnScopeExitTest, DeferThrowsException) {
    std::cout << "[Test] DeferThrowsException (EXPECT_DEATH)" << std::endl;
    EXPECT_DEATH(
            {
                {
                    LOG_DEFER {
                            std::cout << ">> defer throws inside scope (DeferThrowsException)" << std::endl;
                            throw std::runtime_error("Exception in defer");
                    };
                }
            },
            "Exception in defer"
    );
}

// 测试多个 defer 的执行顺序：后进先出（LIFO）
TEST(ExecuteOnScopeExitTest, ExecutionOrder) {
    std::cout << "[Test] ExecutionOrder" << std::endl;
    std::string order;

    {
        LOG_DEFER {
            std::cout << ">> defer 3" << std::endl;
            order += "3";
        };
        LOG_DEFER {
            std::cout << ">> defer 2" << std::endl;
            order += "2";
        };
        LOG_DEFER {
            std::cout << ">> defer 1" << std::endl;
            order += "1";
        };
    }

    std::cout << "Final order: " << order << std::endl;
    EXPECT_EQ(order, "123");
}

// 测试异常安全：确保 defer 在异常情况下仍会执行
TEST(ExecuteOnScopeExitTest, ExceptionSafety) {
    std::cout << "[Test] ExceptionSafety" << std::endl;
    bool executed = false;

    try {
        LOG_DEFER {
            std::cout << ">> defer executed in exception scope" << std::endl;
            executed = true;
        };
        throw std::runtime_error("Test exception");
    } catch (const std::runtime_error& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        EXPECT_TRUE(executed);
    }
}

// 测试捕获不同类型的变量
TEST(ExecuteOnScopeExitTest, VariableCapture) {
    std::cout << "[Test] VariableCapture" << std::endl;
    int value = 10;
    std::unique_ptr<int> ptr = std::make_unique<int>(20);

    {
        LOG_DEFER {
            std::cout << ">> defer modifies captured variables" << std::endl;
            value *= 2;
            *ptr = 40;
        };
    }

    EXPECT_EQ(value, 20);
    EXPECT_EQ(*ptr, 40);
}

// 测试不使用宏，直接使用类的情况
TEST(ExecuteOnScopeExitTest, DirectUsageWithoutMacro) {
    std::cout << "[Test] DirectUsageWithoutMacro" << std::endl;
    bool executed = false;

    {
        auto defer = logger::ExecuteOnScopeExit([&] {
            std::cout << ">> direct usage defer executed" << std::endl;
            executed = true;
        });
    }

    EXPECT_TRUE(executed);
}

// 测试空 defer（无操作）
TEST(ExecuteOnScopeExitTest, EmptyDefer) {
    std::cout << "[Test] EmptyDefer" << std::endl;
    {
        LOG_DEFER {
            std::cout << ">> empty defer (noop)" << std::endl;
        };
    }
    SUCCEED();
}

