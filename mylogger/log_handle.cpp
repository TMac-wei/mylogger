/**
 * @file      log_handle.cpp
 * @brief     [日志过滤分发器核心实现]
 * @author    Weijh
 * @version   1.0
 */

#include "log_handle.h"

#include "formatter/formatter.h"
#include "sinks/sink.h"

namespace logger {

    /**
     * @brief  利用单个sink（由智能指针管理）创建日志处理器
     * @param  sink
     * @return 无
     *          初始化日志级别，然后在sinks数组中添加此种输出位置
     * */
    LogHandle::LogHandle(logger::LogSinkPtr sink) : log_level_(LogLevel::kInfo) {
        /// 检查空指针
        if (!sink) {
            throw std::invalid_argument("LogSink connot be null");
        }

        sinks_.push_back(std::move(sink));
    }

    /**
     * @brief  利用sinks（由智能指针管理）创建日志处理器
     * @param  sinks
     * @return 无
     *          初始化日志级别，然后在sinks数组中添加此种输出位置
     *          直接使用sinks容器的开始和结束迭代器及逆行构造
     * */
    LogHandle::LogHandle(logger::LogSinkPtrList sinks) : log_level_(LogLevel::kInfo) {
        /// 检验所有sink非空
        for (const auto& sink : sinks) {
            if (!sink) {
                throw std::invalid_argument("LogSink cannot be null");
            }
        }
        /// 批量插入sinks
        sinks_.insert(sinks_.end(), sinks.begin(), sinks.end());
    }

    /**
     * @brief  设置日志级别
     * @param  log_level
     * @return 无
     *          设置当前日志级别，日志级别为原子变量（std::atomic），支持多线程安全修改
     *          低于当前级别的日志会被过滤（如设置为 INFO 时，DEBUG 日志会被忽略）
     * */
    void LogHandle::SetLogLevel(logger::LogLevel log_level) {
        log_level_ = log_level;
    }

    /**
     * @brief  获取日志级别
     * @param  无
     * @return 当前日志的级别
     *          获取当前日志的级别
     * */
    LogLevel LogHandle::GetLogLevel() const {
        return log_level_;
    }

    /**
     * @brief  判断是否需要记录日志
     * @param  level
     * @return true or false
     *          根据日志的级别和是否有sink(sinks_不为空）判断是否需要记录下这条日志
     * */
    bool LogHandle::ShouldLog_(LogLevel level) const noexcept {
        return level >= log_level_ && !sinks_.empty();
    }

    /**
     * @brief  将 LogMsg 分发给所有注册的 LogSink
     * @param  log_msg
     * @return 无
     *         遍历sinks_列表，当LogMsg分发给所有注册的LogSink
     * */
    void LogHandle::Log_(const logger::LogMsg &log_msg) {
        for (auto &sink: sinks_) {
            sink->Log(log_msg);
        }
    }

    /**
    * @brief  记录一条日志
    * @param  log_level, loc, message
    * @return 无
    *         通过ShouldLog_判断是否需要记录
     *        构造LogMsg对象
     *        通过Log_将日志分发给所有LosSink
    * */
    void LogHandle::Log(logger::LogLevel log_level, logger::SourceLocation loc, logger::String_View message) {
        if (!ShouldLog_(log_level)) {
            return;
        }
        LogMsg msg(loc, log_level, message);
        Log_(msg);
    }
}
