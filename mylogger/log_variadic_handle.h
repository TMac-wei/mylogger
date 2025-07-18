/**
 * @file      log_variadic_handle.h
 * @brief     [日志处理类VariadicLogHandle，继承自LogHandle，提供了可变参数模板的日志记录功能]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_handle.h"

namespace logger {
    class VariadicLogHandle : public LogHandle {
    public:
        using LogHandle::LogHandle;

        /**
         * 模板函数：带位置信息的日志记录函数
         * @tparam Args
         * @param loc
         * @param log_level
         * @param fmt
         * @param args
         */
        template<typename... Args>
        void Log(SourceLocation loc, LogLevel log_level, fmt::format_string<Args...> fmt, Args &&...args) {
            Log_(loc, log_level, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void Log(LogLevel log_level, fmt::format_string<Args...> fmt, Args &&...args) {
            Log(SourceLocation{}, log_level, fmt, std::forward<Args>(args)...);
        }


    private:
        /***
         * 格式化与日志过滤
         * @tparam Args  可变长参数
         * @param loc   日志位置
         * @param loglevel  日志级别
         * @param fmt
         * @param args
         */
        template<typename... Args>
        void Log_(SourceLocation loc, LogLevel loglevel, fmt::basic_string_view<char> fmt, Args &&... args) {
            if (!ShouldLog_(loglevel)) {
                return;
            }

            /// 栈上分配 256 字节的缓冲区，用于格式化日志内容
            fmt::basic_memory_buffer<char, 256> buf;
            /// 将格式化字符串和参数写入缓冲区，若内容超过 256 字节，fmt库会自动扩展缓冲区到堆上
            fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(std::forward<Args>(args)...));

            /// 构建日志消息结构体
            LogMsg msg(loc, loglevel, String_View(buf.data(), buf.size()));
            LogHandle::Log_(msg);
        }
    };
}
