/**
 * @file      time_count.h
 * @brief     [计时器-测量代码块的执行时间]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "internal_log.h"
#include "third_party/fmt/include/fmt/format.h"

#include <chrono>
#include <string>
#include <iostream>

namespace logger {

    /// 计时器类
    class TimeCount {
    public:
        TimeCount(std::string_view info) : info_(info), start_(std::chrono::steady_clock::now()) {}

        ~TimeCount() {
            /// 结束时间
            auto end = std::chrono::steady_clock::now();
            /// 计算运行时间
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
            LOG_INFO(fmt::format("{} took {} microseconds", info_, diff.count()));
        }

    private:
        std::string_view info_;                                         /// 存储代码块的描述信息
        std::chrono::time_point<std::chrono::steady_clock> start_;      /// 存储开始计时的时间点
    };
}

/// 辅助宏
#define _TIME_CONCAT(a, b) a##b
/// 创建TimerCount对象。当前定义为void(0)，即空操作，这意味着计时器功能被禁用
//#define TIME_COUNT(info) void(0)
/// 启动计时器
#define TIME_COUNT(info) logger::TimeCount _TIME_CONCAT(time_count, __LINE__)(info);

