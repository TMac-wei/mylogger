/**
 * @file      internal_log.h
 * @brief     [调试时的日志打印信息封装，采用条件编译的方式，若启用ENABLE_LOG则打印出对应的系统日志，否则不会打印]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

/// 采用条件编译的方式
#ifdef ENABLE_LOG  /// 只有开启了ENABLE_LOG之后才会打印系统内部的日志信息

/// 启用了 ENBALE_LOG
#include "third_party/fmt/include/fmt/chrono.h"
#include "third_party/fmt/include/fmt/core.h"

/// 定义打印 DEBUG  INFO WARN的宏
#define LOG_INFO(...) \
    fmt::print("[INFO] [{} : {}] {} {}\n", __FILE__, __LINE__, std::chrono::system_clock::now(), fmt::format(__VA_ARGS__))

#define LOG_DEUBG(...) \
    fmt::print("[DEBUG] [{} : {}] {} {}\n", __FILE__, __LINE__, std::chrono::system_clock::now(), fmt::format(__VA_ARGS__))

#define LOG_WARN(...) \
    fmt::print("[WARN] [{} : {}] {} {}\n", __FILE__, __LINE__, std::chrono::system_clock::now(), fmt::format(__VA_ARGS__))

#define LOG_ERROR(...) \
    fmt::print("[ERROR] [{} : {}] {} {}\n", __FILE__, __LINE__, std::chrono::system_clock::now(), fmt::format(__VA_ARGS__))

#else
/// 未启用ENBALE_LOG则执行下方代码
#define LOG_INFO(...)

#define LOG_DEUBG(...)

#define LOG_WARN(...)

#define LOG_ERROR(...)
#endif

