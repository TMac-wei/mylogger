/**
 * @file      decode_test.cpp
 * @brief     [decode_test]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include <string>
#include "decode/decode_formatter.h"

// 测试用例类
class DecodeFormatterTest : public testing::Test {
protected:
    logger::DecodeFormatter formatter_;
    EffectiveMsg test_msg_;

    void SetUp() override {
        // 初始化测试消息
        test_msg_.set_level(2);       // 对应"I"
        test_msg_.set_timestamp(1620000000123); // 带毫秒的时间戳
        test_msg_.set_pid(1234);
        test_msg_.set_tid(5678);
        test_msg_.set_file_name("test_file.cpp");
        test_msg_.set_func_name("TestFunction");
        test_msg_.set_line(100);
        test_msg_.set_log_info("this is a test log");
    }

    // 打印测试结果的辅助函数
    void PrintTestResult(const std::string& test_name,
                         const std::string& expected,
                         const std::string& actual) {
        std::cout << "\n===== " << test_name << " =====" << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        std::cout << "Actual  : " << actual << std::endl;
    }
};

// 测试默认格式
TEST_F(DecodeFormatterTest, DefaultFormat) {
    std::string dest;
    formatter_.Format(test_msg_, dest);

    // 预期格式: [%d][%lld][%d:%d][%s:%s:%d]log_info\n
    std::string expected = "[2][1620000000123][1234:5678][test_file.cpp:TestFunction:100]this is a test log\n";
    PrintTestResult("DefaultFormat", expected, dest);
    EXPECT_EQ(dest, expected);
}

// 测试自定义格式1: 完整模板
TEST_F(DecodeFormatterTest, CustomFormatFull) {
    std::string pattern = "[%l][%D:%S:%M][%p:%t][%F:%f:%#]%v";
    formatter_.SetPattern(pattern);

    std::string dest;
    formatter_.Format(test_msg_, dest);

    // 预期格式: [I][2021-05-03 00:00:00:1620000000:1620000000123][1234:5678][test_file.cpp:TestFunction:100]this is a test log\n
    std::string expected = "[I][2021-05-03 00:00:00:1620000000:1620000000123][1234:5678][test_file.cpp:TestFunction:100]this is a test log\n";
    PrintTestResult("CustomFormatFull", expected, dest);
    EXPECT_EQ(dest, expected);
}

// 测试日志级别格式化
TEST_F(DecodeFormatterTest, LogLevelFormat) {
    std::vector<std::pair<int32_t, std::string>> level_tests = {
            {0, "V"}, {1, "D"}, {2, "I"}, {3, "W"}, {4, "E"}, {5, "F"}, {6, "U"}
    };

    for (auto& test : level_tests) {
        test_msg_.set_level(test.first);
        formatter_.SetPattern("%l");

        std::string dest;
        formatter_.Format(test_msg_, dest);
        std::string expected = test.second + "\n";

        PrintTestResult("LogLevel_" + std::to_string(test.first), expected, dest);
        EXPECT_EQ(dest, expected);
    }
}

// 测试时间格式
TEST_F(DecodeFormatterTest, TimeFormats) {
    // %D 日期时间格式
    formatter_.SetPattern("%D");
    std::string dest_d;
    formatter_.Format(test_msg_, dest_d);
    std::string expected_d = "2021-05-03 00:00:00\n";
    PrintTestResult("TimeFormat_D", expected_d, dest_d);
    EXPECT_EQ(dest_d, expected_d);

    // %S 秒级时间戳
    formatter_.SetPattern("%S");
    std::string dest_s;
    formatter_.Format(test_msg_, dest_s);
    std::string expected_s = "1620000000\n";
    PrintTestResult("TimeFormat_S", expected_s, dest_s);
    EXPECT_EQ(dest_s, expected_s);

    // %M 毫秒级时间戳
    formatter_.SetPattern("%M");
    std::string dest_m;
    formatter_.Format(test_msg_, dest_m);
    std::string expected_m = "1620000000123\n";
    PrintTestResult("TimeFormat_M", expected_m, dest_m);
    EXPECT_EQ(dest_m, expected_m);
}

// 测试进程/线程ID格式
TEST_F(DecodeFormatterTest, ProcessThreadIdFormat) {
    formatter_.SetPattern("[%p:%t]");
    std::string dest;
    formatter_.Format(test_msg_, dest);
    std::string expected = "[1234:5678]\n";

    PrintTestResult("ProcessThreadIdFormat", expected, dest);
    EXPECT_EQ(dest, expected);
}

// 测试文件/函数/行号格式
TEST_F(DecodeFormatterTest, FileFuncLineFormat) {
    formatter_.SetPattern("%F:%f:%#");
    std::string dest;
    formatter_.Format(test_msg_, dest);
    std::string expected = "test_file.cpp:TestFunction:100\n";

    PrintTestResult("FileFuncLineFormat", expected, dest);
    EXPECT_EQ(dest, expected);
}

// 测试普通字符和转义
TEST_F(DecodeFormatterTest, PlainTextAndEscape) {
    // 普通字符测试
    formatter_.SetPattern("log: %v");
    std::string dest1;
    formatter_.Format(test_msg_, dest1);
    std::string expected1 = "log: this is a test log\n";

    // 转义%测试
    formatter_.SetPattern("%%%l");
    std::string dest2;
    formatter_.Format(test_msg_, dest2);
    std::string expected2 = "%I\n";

    PrintTestResult("PlainTextFormat", expected1, dest1);
    PrintTestResult("EscapePercentFormat", expected2, dest2);

    EXPECT_EQ(dest1, expected1);
    EXPECT_EQ(dest2, expected2);
}

//int main(int argc, char **argv) {
//    testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}