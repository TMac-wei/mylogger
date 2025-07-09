/**
 * @file      sys_util_linux.cpp
 * @brief     [linux下获取pagesize、 pid、 tid、 时间]
 * @author    Weijh
 * @version   1.0
 */

#include "utils/sys_util.h"

#include <thread>
#include <unistd.h>

namespace logger {
    /// 获取page_size
    size_t GetPageSize() {
        return getpagesize();
    }

    /// 调用底层api GetCurrentProcessId（）获取进程ID
    size_t GetProcessId() {
        return static_cast<size_t>(::getpid());
    }

    /// 调用底层api GetCurrentThreadId（）获取线程ID
    size_t GetThreadId() {
        return static_cast<size_t>(::gettid());
    }

    /// 结构化时间  tm--> 时间结构体   typedef __time64_t time_t;
    /// errno_t localtime_s(struct tm *result, const time_t *timer);将时间戳转换为本地时间的安全版本
    void LocalTime(std::tm *tm, std::time_t *now) {
        localtime_r(tm, now);   /// tm->存储转换后的本地时间信息； now获取的当前时间
    }
}