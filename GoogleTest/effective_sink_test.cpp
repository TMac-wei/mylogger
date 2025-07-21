/**
 * @file      effective_sink_test.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include <string>

// 被测试的简单函数
int Add(int a, int b) { return a + b; }

bool IsEven(int num) { return num % 2 == 0; }

std::string GetName() { return "GoogleTest"; }

// 基本测试用例
TEST(BasicTest, AddFunction) {
std::cout << "[ INFO ] Running AddFunction test" << std::endl;

// 普通断言
EXPECT_EQ(Add(2, 3), 5) << "Addition failed";

// 布尔断言
EXPECT_TRUE(IsEven(4)) << "4 should be even";
EXPECT_FALSE(IsEven(5)) << "5 should be odd";

// 字符串断言
EXPECT_STREQ(GetName().c_str(), "GoogleTest") << "Name mismatch";

// 浮点数断言
EXPECT_NEAR(3.14, 3.14159, 0.002) << "Pi approximation is off";

std::cout << "[ INFO ] AddFunction test passed" << std::endl;
}

// 参数化测试
struct TestParam {
    int input;
    bool expected;
};

class IsEvenTest : public testing::TestWithParam<TestParam> {};

TEST_P(IsEvenTest, CheckEven) {
auto param = GetParam();
std::cout << "[ INFO ] Testing IsEven(" << param.input << ")" << std::endl;
EXPECT_EQ(IsEven(param.input), param.expected);
}

INSTANTIATE_TEST_SUITE_P(
        EvenOddValues,
        IsEvenTest,
        testing::Values(
        TestParam{2, true},
        TestParam{3, false},
        TestParam{0, true},
        TestParam{-4, true}
)
);

// 死亡测试（验证程序崩溃或异常）
void ThrowException() {
    throw std::runtime_error("Exception occurred");
}

TEST(DeathTest, ExceptionCheck) {
std::cout << "[ INFO ] Running exception check test" << std::endl;
EXPECT_THROW(ThrowException(), std::runtime_error);
std::cout << "[ INFO ] Exception check passed" << std::endl;
}

//int main(int argc, char **argv) {
//    std::cout << "[ INFO ] Starting GoogleTest suite" << std::endl;
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}