/**
 * @file      log_msg.h
 * @brief     [日志结构体]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_commom.h"

namespace logger {
    /// 构建日志消息结构体，每一个日志的最小单元就是log_msg结构体,结构体内部封装了文件名、行号、函数名等信息
    struct LogMsg {
        LogMsg(SourceLocation location, LogLevel level, String_View msg);

        LogMsg(LogLevel level, String_View msg);

        /// 默认支持拷贝构造和赋值
        LogMsg(const LogMsg &) = default;

        LogMsg &operator=(const LogMsg &) = default;

        SourceLocation location_;           /// 文件源位置
        LogLevel loglevel_;                 /// 日志属于哪种级别
        String_View message;                /// 日志消息内容
    };
}


