/**
 * @file      sys_util.h
 * @brief     [获取系统数据，包括页大小、进程ID、线程ID、时间等]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace logger {
    /// 获取pagesize
    size_t GetPageSize();

    /// 获取进程ID
    size_t GetProcessId();

    /// 获取线程ID
    size_t GetThreadId();

    /// 结构化时间  tm--> 时间结构体   typedef __time64_t time_t;
    void LocalTime(std::tm *tm, std::time_t *now);
}


