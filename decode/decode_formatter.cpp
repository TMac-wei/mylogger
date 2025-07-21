/**
 * @file      decode_formatter.cpp
 * @brief     [日志解析格式化器具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "decode_formatter.h"
//#include "mylogger/proto/effective_msg.pb.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

/**
 * 自定义格式化器的格式 模板：[%l][%D:%S][%p:%t][%F:%f:%#]%v
 * 以上内容分别表示：
 * %l log_level
 * %D timestamp date
 * %S timestamp seconds
 * %M timestamp milliseconds
 * %p process_id
 * %t thread_id
 * %# line
 * %F file_name
 * %f func_name
 * %v log_info
 *
 */

class FlagFormatter {
public:
    FlagFormatter() = default;
    virtual ~FlagFormatter() = default;

    /// 把 EffectiveMsg 中的特定字段添加到输出字符串里
    virtual void Format(const EffectiveMsg& flag, std::string& dest) = 0;
};

/// 模式编译: CompilePattern 和 HandleFlag 函数，能够把模式字符串转化为格式化器链表。
static void CompilePattern(const std::string& pattern);
static void HandleFlag(char flag);

/// 具体的格式化器智能指针数组
std::vector<std::unique_ptr<FlagFormatter>> flag_formatters_;

/**
 * @brief 获取日志对应的首字符
 */
static const char* GetLogLevelStr(int32_t loglevel) {
    /// 根据输入的loglevel,返回其对应的不同的日志级别
    switch (loglevel) {
        case 0:
            return "V";
        case 1:
            return "D";
        case 2:
            return "I";
        case 3:
            return "W";
        case 4:
            return "E";
        case 5:
            return "F";
        default:
            return "U";
    }
}

/***
 * @brief 当没有自定义模式时，会使用 CombineLogMsg 函数生成默认格式的日志。
 * @param msg
 * @return 默认格式的日志 [%d][%lld][%d:%d][%s:%s:%d]
 */
std::string CombineLogMsg(const EffectiveMsg& msg) {
    /// 静态固定大小缓冲区可能导致缓冲区溢出
//    std::string output;
//    char buffer[1024] = {0};
//    snprintf(buffer, sizeof(buffer), "[%d][%lld][%d:%d][%s:%s:%d]", msg.level(), msg.timestamp(), msg.pid(),
//            msg.tid(), msg.file_name().c_str(), msg.func_name().c_str(), msg.line());
//    /// 以追加方式添加到输出缓冲区
//    output.append(buffer);
//    output.append(msg.log_info());
//    return output;
    /// 使用动态缓冲区
    int len = snprintf(nullptr, 0, "[%d][%lld][%d:%d][%s:%s:%d]",
                       msg.level(), msg.timestamp(), msg.pid(),
                       msg.tid(), msg.file_name().c_str(), msg.func_name().c_str(), msg.line());

    if (len <= 0) {
        return "";  /// 处理错误
    }

    /// 动态分配足够大小的空间
    std::string buffer(len + 1, '\0');
    snprintf(&buffer[0], buffer.size(), "[%d][%lld][%d:%d][%s:%s:%d]",
             msg.level(), msg.timestamp(), msg.pid(),
             msg.tid(), msg.file_name().c_str(), msg.func_name().c_str(), msg.line());
    buffer.pop_back();
    buffer.append(msg.log_info());
    return buffer;
}

/**
 * @brief 设置模式，并且调用底层CompilePattern对模式进行解析
 * @param pattern
 */
void logger::DecodeFormatter::SetPattern(const std::string& pattern) {
    CompilePattern(pattern);
}

/**
 * @brief 依次调用自定义格式化器进行格式化，若没有自定义格式化匹配的值则使用默认
 * @param msg 日志消息
 * @param dest 输出缓冲区
 */
void logger::DecodeFormatter::Format(const EffectiveMsg &msg, std::string &dest) {
    if (!flag_formatters_.empty()) {
        /// 有自定义格式化器
        for (auto& formatter : flag_formatters_) {
            formatter->Format(msg, dest);
        }
    } else {
        /// 没有自定义格式化器
        dest.append(CombineLogMsg(msg));
    }
    dest.append("\n");
}

/**
 * @brief AggregateFormatter用于收集普通字符
 */
class AggregateFormatter final : public FlagFormatter {
public:
    AggregateFormatter() = default;
    ~AggregateFormatter() = default;

    /// 解析模式字符串时，遇到了非%的普通字符串时调用此方法
    void Addchar(char ch) {
        str_ += ch;
    }

    /// 内部存储的字符串 str_ 追加到目标输出字符串 dest 中
    void Format(const EffectiveMsg& msg, std::string &dest) override {
        dest.append(str_);
    }
private:
    std::string str_;       /// 存储字符串序列，用于收集字符，然后传递给格式化器
};

/**
 * @brief 日志级别格式化器
 */
class LogLevelFormatter final : public FlagFormatter {
public:
    LogLevelFormatter() = default;
    ~LogLevelFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string & dest) override {
        dest.append(GetLogLevelStr(msg.level()));
    }
};

/**
 * @brief 用于将时间戳（毫秒级别）转换为格式化日期时间字符串（ YYYY-MM-DD HH:MM:SS）
 * @return 转换后的日期时间字符串
 *      主要转换步骤： 1、将毫秒转换为system_clock::time_point
 *                  2、将 time_point 转换为 std::time_t
 *                  3、将 time_t 转换为本地时间的 std::tm 结构体
 *                  4、格式化 tm 结构体为字符串
 */
 std::string MillisecondsToDateString(long long milliseconds) {
     std::chrono::system_clock::time_point timePoint =
             std::chrono::system_clock::time_point(std::chrono::milliseconds(milliseconds));
     std::time_t time_tt = std::chrono::system_clock::to_time_t(timePoint);
//     std::tm* local_time = std::localtime(&time_tt);
    std::tm* utc_time = std::gmtime(&time_tt);
//     std::tm timeinfo;
//     localtime_s(&time_tt, timeinfo);

     std::ostringstream oss;
//     oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    oss << std::put_time(utc_time, "%Y-%m-%d %H:%M:%S");
     return oss.str();
 }

 /**
  * 提供三种时间戳转换的格式化器
  * @brief 1、时间戳转换为本地时间
  */
 class TimeStampToLocalDateFormatter final : public FlagFormatter {
 public:
     TimeStampToLocalDateFormatter() = default;
     ~TimeStampToLocalDateFormatter() override = default;

     void Format(const EffectiveMsg& msg, std::string& dest) override {
         /// 将转换后的本地时间格式追加到输出缓冲区
         dest.append(MillisecondsToDateString(msg.timestamp()));
     }
 };

/**
* 提供三种时间戳转换的格式化器
* @brief 2、时间戳转换为秒
*/
class  TimeStampSecondsFormatter final : public FlagFormatter {
public:
    TimeStampSecondsFormatter() = default;
    ~TimeStampSecondsFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string &dest) override {
        dest.append(std::to_string(msg.timestamp() / 1000));
    }
};

/**
* 提供三种时间戳转换的格式化器
* @brief 3、原始的毫秒级别
*/
class  TimeStampMillisecondsFormatter final : public FlagFormatter {
public:
    TimeStampMillisecondsFormatter() = default;
    ~TimeStampMillisecondsFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(std::to_string(msg.timestamp()));
    }
};

/**
 * @brief 进程ID格式化器
 */
class ProcessIdFormatter final : public FlagFormatter {
public:
    ProcessIdFormatter() = default;
    ~ProcessIdFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(std::to_string(msg.pid()));
    }
};

/**
 * @brief 线程ID格式化器
 */
class ThreadIdFormatter final : public FlagFormatter {
public:
    ThreadIdFormatter() = default;
    ~ThreadIdFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(std::to_string(msg.tid()));
    }
};

/**
 * @brief 行号格式化器
 */
class LineFormatter final : public FlagFormatter {
public:
    LineFormatter() = default;
    ~LineFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(std::to_string(msg.line()));
    }
};

/**
 * @brief 文件名格式化器
 */
class FileNameFormatter final : public FlagFormatter {
public:
    FileNameFormatter() = default;
    ~FileNameFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(msg.file_name());
    }
};

/**
 * @brief 函数件名格式化器
 */
class FuncNameFormatter final : public FlagFormatter {
public:
    FuncNameFormatter() = default;
    ~FuncNameFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override{
        dest.append(msg.func_name());
    }
};

/**
 * @brief 日志消息内容格式化器
 */
class LogInfoFormatter final : public FlagFormatter {
public:
    LogInfoFormatter() = default;
    ~LogInfoFormatter() override = default;

    void Format(const EffectiveMsg& msg, std::string& dest) override {
        dest.append(msg.log_info());
    }
};

/**
 * @brief 解析模式字符串，识别普通字符和格式化标志（以 % 开头），并创建对应的格式化器对象
 * @param pattern 模式字符串
 * 核心逻辑：
 *      1、清空现有的格式化器列表flag_formatters_
 *      2、遍历模式字符串，逐个字符串解析，区分普通字符和格式化字符
 *      3、将普通字符添加到AggregateFormatter
 *      4、遇到 % 处理格式化字符，暂停AggregateFormatter收集普通字符，根据后序的字符调用HandleFlag创建对应的格式化器
 *      5、收尾
 */
void CompilePattern(const std::string& pattern) {
    /// 找到模式字符尾迭代器并且创建一个普通字符收集器
    auto end = pattern.end();
    std::unique_ptr<AggregateFormatter> user_normal_chars;

    flag_formatters_.clear();

    for (auto it = pattern.begin(); it != end; ++it) {
        if (*it == '%') {
            /// 创建特定的格式化器
            if (user_normal_chars) {
                flag_formatters_.push_back(std::move(user_normal_chars));
            }
            ++it;
            if (it == end) {
                /// 处理末尾的%：添加一个包含%的AggregateFormatter
                auto formatter = std::make_unique<AggregateFormatter>();
                formatter->Addchar('%');  /// 只添加 %
                flag_formatters_.push_back(std::move(formatter));
                break;
            }

            /// 若下一个字符还是 % 处理为单个 %
            if (*it == '%') {
                auto formatter = std::make_unique<AggregateFormatter>();
                formatter->Addchar('%');
                flag_formatters_.push_back(std::move(formatter));
            } else {
                HandleFlag(*it);
            }
        } else {
            if (!user_normal_chars) {
                user_normal_chars = std::make_unique<AggregateFormatter>();
            }
            user_normal_chars->Addchar(*it);
        }
    }
    /// 如果字符收集器中还有剩余，则继续添加
    if (user_normal_chars) {
        flag_formatters_.push_back(std::move(user_normal_chars));
    }
}

/**
 * @brief 根据格式化标志字符串，创建特定类型的格式化器
 * @param flag 标志
 */
 void HandleFlag(char flag) {
    switch (flag) {
        case 'l':
            flag_formatters_.push_back(std::make_unique<LogLevelFormatter>());
            break;
        case 'D':
            flag_formatters_.push_back(std::make_unique<TimeStampToLocalDateFormatter>());
            break;
        case 'S':
            flag_formatters_.push_back(std::make_unique<TimeStampSecondsFormatter>());
            break;
        case 'M':
            flag_formatters_.push_back(std::make_unique<TimeStampMillisecondsFormatter>());
            break;
        case 'p':
            flag_formatters_.push_back(std::make_unique<ProcessIdFormatter>());
            break;
        case 't':
            flag_formatters_.push_back(std::make_unique<ThreadIdFormatter>());
            break;
        case 'F':
            flag_formatters_.push_back(std::make_unique<FileNameFormatter>());
            break;
        case 'f':
            flag_formatters_.push_back(std::make_unique<FuncNameFormatter>());
            break;
        case '#':
            flag_formatters_.push_back(std::make_unique<LineFormatter>());
            break;
        case 'v':
            flag_formatters_.push_back(std::make_unique<LogInfoFormatter>());
            break;
        default:
            /// 处理 % 跟任意字符的情况，但是无法处理 % 作为最后一个字符
            auto formatter = std::make_unique<AggregateFormatter>();
            if (flag != '%') {  /// 若 flag 是 %，直接添加一个 %
                formatter->Addchar('%');
            }
            formatter->Addchar(flag);
            flag_formatters_.push_back(std::move(formatter));
            break;
    }
 }