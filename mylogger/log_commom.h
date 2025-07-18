/**
 * @file      log_commom.h
 * @brief     [日志库的基础组件]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "third_party/fmt/include/fmt/format.h"

/// 定义日志级别的宏定义，数字越大级别越高
#define LOGGER_LEVEL_TRACE 0
#define LOGGER_LEVEL_DEBUG 1
#define LOGGER_LEVEL_INFO 2
#define LOGGER_LEVEL_WARN 3
#define LOGGER_LEVEL_ERROR 4
#define LOGGER_LEVEL_CRITICAL 5
#define LOGGER_LEVEL_OFF 6

namespace logger {

    /// 基础工具的类型定义，string_view是一个轻量级的字符串试图，可以避免不必要的拷贝，同时使用string作为内存缓存区，用于构建格式化后的日志消息
    using String_View = std::string_view;
    using MemoryBuf = std::string;

    /// FormatString模板是基于fmt封装的类型安全格式字符串，可以在编译时检查格式字符串与参数类型是否匹配
    template<typename... Args>
    using FormatString = fmt::format_string<Args...>;

    /// 定义一个移除cv引用的类型别名,用于获取类型的原始形式
    template<typename T>
    using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

    /// 类型特性工具
    /// is_convertible_to_basic_format_string：检测类型是否可转换为fmt格式字符串
    template<typename T, typename Char = char>
    struct is_convertible_to_basic_format_string
            : std::integral_constant<bool,
                    /// 支持两种情况：直接可转换为string_view或为runtime_format_string类型
                    std::is_convertible<T, fmt::basic_string_view<Char>>::value ||
                    std::is_same<remove_cvref_t<T>, fmt::runtime_format_string<Char>>::value> {
    };

    /// 转换成任意的格式化字符串类型,同时支持char和wchar_t两种字符类型
    template<typename T>
    struct is_convertible_to_any_format_string
            : std::integral_constant<bool,
                    is_convertible_to_basic_format_string<T, char>::value ||
                    is_convertible_to_basic_format_string<T, wchar_t>::value> {
    };

    /// 枚举日志级别
    enum class LogLevel {
        kTrace = LOGGER_LEVEL_TRACE,        /// 最详细的调试信息
        kDebug = LOGGER_LEVEL_DEBUG,        /// 开发的调试信息
        kInfo = LOGGER_LEVEL_INFO,          /// 正常运行信息
        kWarn = LOGGER_LEVEL_WARN,          /// 告警
        kError = LOGGER_LEVEL_ERROR,        /// 报错细信息
        kFatal = LOGGER_LEVEL_CRITICAL,     /// 致命错误
        kOff = LOGGER_LEVEL_OFF             /// 关闭所有日志
    };

    /// LOGGER_ACTIVE_LEVEL定义当前激活的最低日志级别
#define LOGGER_ACTIVE_LEVEL LOGGER_LEVEL_TRACE

    /// 源位置信息记录结构体，里面包含了日志调用的文件、行号和函数信息
    /// 需要同时处理unix和win下的路径分割符来提取文件名
    struct SourceLocation {
        constexpr SourceLocation() = default;

        /// 通过文件名，行号以及函数名进行构建
        SourceLocation(String_View file_name_in, int32_t line_in, String_View func_name_in)
                : file_name_(file_name_in), line_(line_in), func_name_(func_name_in) {
            /// 分别找到unix和win系统下的路径中的文件名
            if (!file_name_.empty()) {
                size_t pos = file_name_.rfind('/'); /// 从右边开始找
                if (pos != String_View::npos) {
                    /// 匹配到了
                    file_name_ = file_name_.substr(pos + 1);
                } else {
                    pos = file_name_.rfind('\\');
                    if (pos != String_View::npos) {
                        file_name_ = file_name_.substr(pos + 1);
                    }
                }
            }
        }

        String_View file_name_;         /// 代表日志的输入文件
        int32_t line_{0};              /// 代表输入行号
        String_View func_name_;         /// 代表日志输入函数
    };
}


