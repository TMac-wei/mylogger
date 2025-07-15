/**
 * @file      log_handle.h
 * @brief     [LogHandle 是日志框架中的核心类，负责处理日志消息的过滤和分发]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_commom.h"
#include "log_msg.h"

#include <atomic>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace logger {
    class LogSink;
    using LogSinkPtr = std::shared_ptr<LogSink>;
    using LogSinkPtrList = std::initializer_list<LogSinkPtr>;

    class LogHandle {
    public:

        /// 初始化，构造函数，提供三种方式初始化
        explicit LogHandle(LogSinkPtr sink);
        explicit LogHandle(LogSinkPtrList sinks);

        /// 迭代器方式构造LogHandle,迭代器范围，用于从容器（如 vector）初始化
        template<typename It>
        LogHandle(It begin, It end) : log_level_(LogLevel::kInfo), sinks_(begin, end){}

        /// 删除拷贝构造和赋值运算符，避免资源重复管理
        LogHandle(const LogHandle&) = delete;
        LogHandle& operator=(const LogHandle&) = delete;

        /// 日志级别控制，设置日志级别以及获取当前日志级别
        void SetLogLevel(LogLevel log_level);

        LogLevel GetLogLevel() const;

        /// 日志的输出接口，
        void Log(LogLevel log_level, SourceLocation loc, String_View message);

    protected:
        /// 判断是否需要记录，级别是否足够且有sink
        bool ShouldLog_(LogLevel level) const noexcept;

        /// 将 LogMsg 分发给所有注册的 LogSink,遍历 sinks_ 列表，调用每个 LogSink 的输出方法
        void Log_(const LogMsg& log_msg);

    private:
        std::atomic<LogLevel> log_level_;       /// 日志级别
        std::vector<LogSinkPtr> sinks_;         /// 输出位置
        mutable std::mutex sink_mutex_;         /// 确保多线程访问Sink时，线程安全
    };
}

