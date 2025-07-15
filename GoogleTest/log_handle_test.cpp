/**
 * @file      log_handle_test.cpp
 * @brief     [日志库核心组件的单元测试（带详细打印）]
 * @author    Weijh
 * @version   1.2
 */

#include "gtest/gtest.h"
#include "log_handle.h"
#include "sinks/console_sink.h"
#include "formatter/default_formatter.h"
#include "formatter/formatter.h"
#include "log_msg.h"
#include "log_commom.h"

#include <memory>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdio>

// 辅助工具：捕获标准输出和标准错误
class StdoutCapture {
public:
    StdoutCapture() {
        // 保存原始流缓冲区
        old_stdout_ = std::cout.rdbuf();
        old_stderr_ = std::cerr.rdbuf();

        // 重定向cout和cerr到stringstream
        std::cout.rdbuf(buffer_.rdbuf());
        std::cerr.rdbuf(buffer_.rdbuf());

        // 强制刷新所有输出流
        fflush(stdout);
        fflush(stderr);
        std::cout.flush();
        std::cerr.flush();
    }

    ~StdoutCapture() {
        // 恢复原始流缓冲区并刷新
        std::cout.rdbuf(old_stdout_);
        std::cerr.rdbuf(old_stderr_);
        std::cout.flush();
        std::cerr.flush();
    }

    std::string str() const {
        // 确保所有缓冲区内容都被捕获
        return buffer_.str();
    }

private:
    mutable std::stringstream buffer_;
    std::streambuf* old_stdout_;
    std::streambuf* old_stderr_;
};

// 模拟Sink：用于测试日志消息的接收
class MockSink : public logger::LogSink {
public:
    void Log(const logger::LogMsg& msg) override {
        captured_msgs_.push_back(msg);
        std::cout << "[MockSink] 收到日志: " << std::string(msg.message.data(), msg.message.size()) << std::endl;
    }

    void SetFormatter(std::unique_ptr<logger::Formatter> formatter) override {
        formatter_ = std::move(formatter);
        std::cout << "[MockSink] 已设置自定义格式化器" << std::endl;
    }

    const std::vector<logger::LogMsg>& GetCapturedMsgs() const {
        return captured_msgs_;
    }

private:
    std::vector<logger::LogMsg> captured_msgs_;
    std::unique_ptr<logger::Formatter> formatter_;
};

// 测试LogHandle的基本功能
TEST(LogHandleTest, BasicFunctionality) {
    std::cout << "\n===== 开始测试 LogHandleTest.BasicFunctionality =====" << std::endl;

    // 创建模拟Sink
    std::cout << "创建模拟Sink..." << std::endl;
    auto mock_sink = std::make_shared<MockSink>();

    // 创建LogHandle
    std::cout << "创建LogHandle..." << std::endl;
    logger::LogHandle handle(mock_sink);

    // 设置日志级别为INFO
    std::cout << "设置日志级别为INFO..." << std::endl;
    handle.SetLogLevel(logger::LogLevel::kInfo);
    std::cout << "当前日志级别: " << static_cast<int>(handle.GetLogLevel()) << std::endl;

    // 发送一条DEBUG日志（应被过滤）
    std::cout << "发送DEBUG级别日志（预期被过滤）..." << std::endl;
    handle.Log(
            logger::LogLevel::kDebug,
            logger::SourceLocation{__FILE__, __LINE__, "LogHandleTest_BasicFunctionality"},
            "This is a debug message"
    );

    // 发送一条INFO日志（应被记录）
    std::cout << "发送INFO级别日志（预期被记录）..." << std::endl;
    handle.Log(
            logger::LogLevel::kInfo,
            logger::SourceLocation{__FILE__, __LINE__, "LogHandleTest_BasicFunctionality"},
            "This is an info message"
    );

    // 验证结果
    std::cout << "验证日志接收结果..." << std::endl;
    const auto& msgs = mock_sink->GetCapturedMsgs();
    EXPECT_EQ(msgs.size(), 1) << "预期收到1条日志，实际收到" << msgs.size() << "条";
    std::cout << "日志数量验证通过: 共收到" << msgs.size() << "条日志" << std::endl;

    EXPECT_EQ(msgs[0].loglevel_, logger::LogLevel::kInfo) << "日志级别不符合预期";
    std::cout << "日志级别验证通过: 实际级别为" << static_cast<int>(msgs[0].loglevel_) << std::endl;

    std::string msg_content(msgs[0].message.data(), msgs[0].message.size());
    EXPECT_EQ(msg_content, "This is an info message") << "日志内容不符合预期";
    std::cout << "日志内容验证通过: " << msg_content << std::endl;

    std::cout << "===== 结束测试 LogHandleTest.BasicFunctionality =====" << std::endl;
}

// 测试多Sink分发
TEST(LogHandleTest, MultipleSinks) {
    std::cout << "\n===== 开始测试 LogHandleTest.MultipleSinks =====" << std::endl;

    // 创建两个模拟Sink
    std::cout << "创建两个模拟Sink..." << std::endl;
    auto sink1 = std::make_shared<MockSink>();
    auto sink2 = std::make_shared<MockSink>();

    // 使用初始化列表构造LogHandle
    std::cout << "使用初始化列表构造LogHandle（包含2个Sink）..." << std::endl;
    logger::LogHandle handle({sink1, sink2});

    // 发送一条日志
    std::cout << "发送一条WARN级别日志..." << std::endl;
    handle.Log(
            logger::LogLevel::kWarn,
            logger::SourceLocation{__FILE__, __LINE__, "LogHandleTest_MultipleSinks"},
            "Warning message"
    );

    // 验证两个Sink都收到了日志
    std::cout << "验证两个Sink的日志接收情况..." << std::endl;
    EXPECT_EQ(sink1->GetCapturedMsgs().size(), 1) << "Sink1未收到日志";
    EXPECT_EQ(sink2->GetCapturedMsgs().size(), 1) << "Sink2未收到日志";
    std::cout << "两个Sink均收到日志，数量验证通过" << std::endl;

    // 验证日志内容相同
    const auto& msg1 = sink1->GetCapturedMsgs()[0];
    const auto& msg2 = sink2->GetCapturedMsgs()[0];
    std::string content1(msg1.message.data(), msg1.message.size());
    std::string content2(msg2.message.data(), msg2.message.size());
    EXPECT_EQ(content1, content2) << "两个Sink收到的日志内容不一致";
    std::cout << "日志内容一致性验证通过: " << content1 << std::endl;

    std::cout << "===== 结束测试 LogHandleTest.MultipleSinks =====" << std::endl;
}

// 测试DefaultFormatter的格式化逻辑
TEST(DefaultFormatterTest, Formatting) {
    std::cout << "\n===== 开始测试 DefaultFormatterTest.Formatting =====" << std::endl;

    // 创建格式化器和MemoryBuf
    std::cout << "创建DefaultFormatter和MemoryBuf..." << std::endl;
    logger::DefaultFormatter formatter;
    logger::MemoryBuf dest;

    // 创建日志消息
    std::cout << "创建一条DEBUG级别日志消息..." << std::endl;
    logger::LogMsg msg(
            logger::SourceLocation{__FILE__, __LINE__, "DefaultFormatterTest_Formatting"},
            logger::LogLevel::kDebug,
            "Formatting test"
    );

    // 执行格式化
    std::cout << "调用Formatter.Format()格式化日志..." << std::endl;
    formatter.Format(msg, &dest);

    // 转换为字符串
    std::string formatted(dest.data(), dest.size());
    std::cout << "格式化后的日志内容:\n" << formatted << std::endl;

    // 验证格式化结果
    EXPECT_NE(formatted.find("[D]"), std::string::npos) << "格式化结果不包含DEBUG级别标识[D]";
    EXPECT_NE(formatted.find("Formatting test"), std::string::npos) << "格式化结果不包含日志内容";
    EXPECT_NE(formatted.find("log_handle_test.cpp"), std::string::npos) << "格式化结果不包含源文件信息";

    std::cout << "DefaultFormatter格式化逻辑验证通过" << std::endl;
    std::cout << "===== 结束测试 DefaultFormatterTest.Formatting =====" << std::endl;
}

// 测试日志级别过滤
TEST(LogHandleTest, LogLevelFiltering) {
    std::cout << "\n===== 开始测试 LogHandleTest.LogLevelFiltering =====" << std::endl;

    auto mock_sink = std::make_shared<MockSink>();
    logger::LogHandle handle(mock_sink);

    // 设置日志级别为WARN
    std::cout << "设置日志级别为WARN（仅WARN及以上级别日志会被记录）..." << std::endl;
    handle.SetLogLevel(logger::LogLevel::kWarn);
    std::cout << "当前日志级别: " << static_cast<int>(handle.GetLogLevel()) << std::endl;

    // 发送不同级别的日志
    std::cout << "发送不同级别的日志（从TRACE到ERROR）..." << std::endl;
    handle.Log(logger::LogLevel::kTrace, logger::SourceLocation{__FILE__, __LINE__, ""}, "Trace message");
    handle.Log(logger::LogLevel::kDebug, logger::SourceLocation{__FILE__, __LINE__, ""}, "Debug message");
    handle.Log(logger::LogLevel::kInfo, logger::SourceLocation{__FILE__, __LINE__, ""}, "Info message");
    handle.Log(logger::LogLevel::kWarn, logger::SourceLocation{__FILE__, __LINE__, ""}, "Warn message");
    handle.Log(logger::LogLevel::kError, logger::SourceLocation{__FILE__, __LINE__, ""}, "Error message");

    // 验证只收到WARN及以上级别的日志
    const auto& msgs = mock_sink->GetCapturedMsgs();
    std::cout << "验证收到的日志数量（预期2条：WARN和ERROR）..." << std::endl;
    EXPECT_EQ(msgs.size(), 2) << "实际收到" << msgs.size() << "条日志，不符合预期";

    std::cout << "验证第一条日志为WARN级别..." << std::endl;
    EXPECT_EQ(msgs[0].loglevel_, logger::LogLevel::kWarn);
    std::cout << "验证第二条日志为ERROR级别..." << std::endl;
    EXPECT_EQ(msgs[1].loglevel_, logger::LogLevel::kError);

    std::cout << "验证日志内容..." << std::endl;
    EXPECT_EQ(std::string(msgs[0].message.data(), msgs[0].message.size()), "Warn message");
    EXPECT_EQ(std::string(msgs[1].message.data(), msgs[1].message.size()), "Error message");

    std::cout << "===== 结束测试 LogHandleTest.LogLevelFiltering =====" << std::endl;
}

// 测试LogHandle的迭代器构造函数
TEST(LogHandleTest, IteratorConstructor) {
    std::cout << "\n===== 开始测试 LogHandleTest.IteratorConstructor =====" << std::endl;

    std::cout << "创建包含2个MockSink的vector..." << std::endl;
    std::vector<std::shared_ptr<logger::LogSink>> sinks;
    sinks.push_back(std::make_shared<MockSink>());
    sinks.push_back(std::make_shared<MockSink>());

    // 使用迭代器构造LogHandle
    std::cout << "使用迭代器范围构造LogHandle..." << std::endl;
    logger::LogHandle handle(sinks.begin(), sinks.end());

    // 发送日志
    std::cout << "发送一条INFO级别日志..." << std::endl;
    handle.Log(
            logger::LogLevel::kInfo,
            logger::SourceLocation{__FILE__, __LINE__, "LogHandleTest_IteratorConstructor"},
            "Iterator test"
    );

    // 验证两个Sink都收到了日志
    auto sink1 = std::dynamic_pointer_cast<MockSink>(sinks[0]);
    auto sink2 = std::dynamic_pointer_cast<MockSink>(sinks[1]);

    std::cout << "验证第一个Sink收到日志..." << std::endl;
    EXPECT_EQ(sink1->GetCapturedMsgs().size(), 1) << "Sink1未收到日志";
    std::cout << "验证第二个Sink收到日志..." << std::endl;
    EXPECT_EQ(sink2->GetCapturedMsgs().size(), 1) << "Sink2未收到日志";

    std::cout << "===== 结束测试 LogHandleTest.IteratorConstructor =====" << std::endl;
}

