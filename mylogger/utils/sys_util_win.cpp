/**
 * @file      sys_util_win.cpp
 * @brief     [win调用api下获取page_size、pid、tid、时间]
 * @author    Weijh
 * @version   1.0
 */

#include "utils/sys_util.h"

#include <windows.h>

#include <sysinfoapi.h>

namespace logger {
    /// 获取page_size
    size_t GetPageSize() {
        SYSTEM_INFO info;           /// SYSTEM_INFO包含一些win的系统信息数据，其中dwPageSize为我们需要的
        GetSystemInfo(&info);
        return info.dwPageSize;
    }

    /// 调用底层api GetCurrentProcessId（）获取进程ID
    size_t GetProcessId() {
        return static_cast<size_t>(::GetCurrentProcessId());
    }

    /// 调用底层api GetCurrentThreadId（）获取线程ID
    size_t GetThreadId() {
        return static_cast<size_t>(::GetCurrentThreadId());
    }

    /// 结构化时间  tm--> 时间结构体   typedef __time64_t time_t;
    /// errno_t localtime_s(struct tm *result, const time_t *timer);将时间戳转换为本地时间的安全版本
    void LocalTime(std::tm *tm, std::time_t *now) {
        localtime_s(tm, now);   /// tm->存储转换后的本地时间信息； now获取的当前时间
    }

    /**
        struct tm {
        int tm_sec;   // 秒 (0-60，允许闰秒)
        int tm_min;   // 分钟 (0-59)
        int tm_hour;  // 小时 (0-23)
        int tm_mday;  // 日 (1-31)
        int tm_mon;   // 月 (0-11，0 表示 1 月)
        int tm_year;  // 年 (当前年份 - 1900)
        int tm_wday;  // 星期几 (0-6，0 表示星期日)
        int tm_yday;  // 一年中的第几天 (0-365)
        int tm_isdst; // 夏令时标志（正数表示使用夏令时）
    };
     * */
}