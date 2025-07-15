/**
 * @file      console_sink.h
 * @brief     [打印到终端]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "sinks/sink.h"

namespace logger {
    /// 打印到终端的类，继承自sink抽象类，基于Strand机制，所有操作都在单线程的线程池中执行，无锁
    class ConsoleSink : public LogSink {
    public:
        ConsoleSink();

        ~ConsoleSink() override = default;

        void Log(const LogMsg& msg) override;

        void SetFormatter(std::unique_ptr<Formatter> formatter) override;

        void Flush() override;  /// 重写Flush，支持显式刷新
    private:
        std::unique_ptr<Formatter> formatter_;          /// 使用智能指针管理格式化器，里面是抽象接口类
    };
}
