/**
 * @file      logger.h
 * @brief     [编译期可配置的日志宏的封装]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_commom.h"
#include "log_factory.h"
#include "log_handle.h"

/// 根据传入的 handle 获取全局单例 logfactory 对象，并且设置全局日志句柄，决定日志如何输出（写文件？打印到控制台？发送到远程？）
/// EXT_LOGGER_INIT 展开后调用 LogFactory::SetLogHandle
#define EXT_LOGGER_INIT(handle) logger::LogFactory::Instance().SetLogHandle(handle)

/// 简化日志调用
#define LOGGER_CALL(handle, level, ...) do {\
  if (handle) {     \
    (handle)->Log(logger::SourceLocation{__FILE__, __LINE__, static_cast<const char*>(__FUNCTION__)}, level, \
        __VA_ARGS__);      \
  } \
} while(0)

/// 基于日志级别控制的宏定义，在不同的日志级别启用不同或禁用对应的日志输出功能
#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_TRACE
/// 启用 TRACE 级别的日志宏,通过指定的 handle 输出 TRACE 级别的日志
#define LOG_LOGGER_TRACE(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kTrace, __VA_ARGS__)
#define EXT_LOG_TRACE(...) LOG_LOGGER_TRACE(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_TRACE(handle, ...) ((void)0) /// 禁用 TRACE 级别的日志
#define EXT_LOG_TRACE(...) ((void)0)

#endif

#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_DEBUG

/// 启用 DEBUG 级别的日志宏
#define LOG_LOGGER_DEBUG(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kDebug, __VA_ARGS__)
#define EXT_LOG_DEBUG(...) LOG_LOGGER_DEBUG(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_DEBUG(handle, ...) ((void)0) /// 禁用 DEBUG 级别的日志
#define EXT_LOG_DEBUG(...) ((void)0)

#endif

#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_INFO
/// 启用 INFO 级别的日志宏
#define LOG_LOGGER_INFO(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kInfo, __VA_ARGS__)
#define EXT_LOG_INFO(...) LOG_LOGGER_INFO(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_INFO(handle, ...) ((void)0) /// 禁用 INFO 级别的日志
#define EXT_LOG_INFO(...) ((void)0)

#endif

#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_WARN
/// 启用 WARN 级别的日志宏
#define LOG_LOGGER_WARN(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kWarn, __VA_ARGS__)
#define EXT_LOG_WARN(...) LOG_LOGGER_WARN(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_WARN(handle, ...) ((void)0) /// 禁用 WARN 级别的日志
#define EXT_LOG_WARN(...) ((void)0)

#endif

#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_ERROR
/// 启用 ERROR 级别的日志宏
#define LOG_LOGGER_ERROR(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kError, __VA_ARGS__)
#define EXT_LOG_ERROR(...) LOG_LOGGER_ERROR(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_ERROR(handle, ...) ((void)0) /// 禁用 ERROR 级别的日志
#define EXT_LOG_ERROR(...) ((void)0)

#endif

#if LOGGER_ACTIVE_LEVEL <= LOGGER_LEVEL_CRITICAL
/// 启用 CRITICAL 级别的日志宏
#define LOG_LOGGER_CRITICAL(handle, ...) LOGGER_CALL(handle, logger::LogLevel::kFatal, __VA_ARGS__)
#define EXT_LOG_CRITICAL(...) LOG_LOGGER_CRITICAL(logger::LogFactory::Instance().GetLogHandle(), __VA_ARGS__)

#else

/// 当日志级别不满足时，将宏定义为空操作，避免编译错误
#define LOG_LOGGER_CRITICAL(handle, ...) ((void)0) /// 禁用 CRITICAL 级别的日志
#define EXT_LOG_CRITICAL(...) ((void)0)

#endif




