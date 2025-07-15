/**
 * @file      console_sink.cpp
 * @brief     [输出到console的具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "sinks/console_sink.h"
#include "formatter/default_formatter.h"

#include <iostream>

namespace logger {

    /**
     * @brief 默认构造
     * */
    ConsoleSink::ConsoleSink() : formatter_(std::make_unique<DefaultFormatter>()) {}

    /**
    * @brief 格式化日志消息
     * @param msg  封装好的日志消息结构体，日志系统中最小的单元为LogMsg
     * @return 无
     *          将消息格式化输出到stdout端，每行输入一个换行
     *          使用异常捕获机制，避免因单个日志错误，导致整个loop崩溃
    * */
    void ConsoleSink::Log(const LogMsg &msg) {
        try {
            MemoryBuf buf;
            formatter_->Format(msg, &buf);
            fwrite(buf.data(), 1, buf.size(), stdout);
            fwrite("\n", 1, 1, stdout);
            fflush(stdout);     /// 显式刷新
            std::cout.flush();  /// 刷新C++的cout
            std::cerr.flush();  /// 刷新C++的cerr
        } catch (const std::exception &e) {
            /// 捕获格式化异常（避免单个日志错误导致整个loop崩溃）
            fprintf(stderr, "ConsoleSink format error: %s\n", e.what());
            fflush(stderr);
        }

    }

    /**
    * @brief 设置格式化日志消息
     * @param formatter  独占式智能指针，内部包含了Formatter
     * @return 无
     *          设置当前的formatter_，需要进行有效性检查，避免传入nullptr导致后序调用出现崩溃
    * */
    void ConsoleSink::SetFormatter(std::unique_ptr<Formatter> formatter) {
        if (!formatter) {
            throw std::invalid_argument("Formatter cannot be null (strand mode)");
        }
        formatter_ = std::move(formatter);
    }

    /**
     * @brief 刷新缓冲区（strand机制下，单线程执行）
     */
    void ConsoleSink::Flush() {
        fflush(stdout);
    }
}