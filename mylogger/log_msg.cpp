/**
 * @file      log_msg.cpp
 * @brief     [log_msg结构体的具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "log_msg.h"

namespace logger {

    /// 利用初始化列表实现
    LogMsg::LogMsg(logger::SourceLocation location, logger::LogLevel level, logger::String_View msg)
            : location_(location), loglevel_(level), message(msg) {}

    /// 显示调用上方构造函数实现
    LogMsg::LogMsg(logger::LogLevel level, logger::String_View msg)
            : LogMsg(SourceLocation{}, level, msg) {}
}