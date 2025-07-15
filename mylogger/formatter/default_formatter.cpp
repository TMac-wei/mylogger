/**
 * @file      default_formatter.cpp
 * @brief     [default_formatter实现]
 * @author    Weijh
 * @version   1.0
 */

#include "formatter/default_formatter.h"
#include "utils/sys_util.h"

namespace logger {

    /**
     * @brief  重载抽象接口的Format方法，这里是默认输出到控制台，日志中还会根据sink输出到不同的输出端
     * @param msg   消息结构体
     *        dest  缓存空间 MemoryBuf-->std::string
     *   把日志消息LogMsg格式化成特定样式的文本，接着将结果存入MemoryBuf类型的缓冲区。
     *  输出格式按照：[2025-01-01 00:00:00.000] [INFO] [file.cpp:11] [pit:tid] message格式
     *  完整的处理流程是：
     *          1、时间戳的格式化
     *          2、日志级别的住那换
     *          3、源码位置信息的添加
     *          4、进程和线程ID的记录
     *          5、原始日志内容的追加
     * */
    void DefaultFormatter::Format(const logger::LogMsg &msg, logger::MemoryBuf *dest) {
        /// 字符T、D、I、W、E、F分别代表TRACE、DEBUG、INFO、WARN、ERROR、FATAL这些日志级别,通过级别的索引值获得对应的字符
        constexpr char kLogLevelStr[] = "TDIWEFO";

        /// 时间戳的格式化
        std::time_t now = std::time(nullptr);
        std::tm tm;
        LocalTime(&tm, &now);
        char time_buf[32] = {0};
        /// 获取系统时间戳，并转换为本地时间之后，利用strftime格式化为需要的字符串形式
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm);
        /// 往缓冲区中添加时间格式
        dest->append("[",1);
        dest->append(time_buf, sizeof time_buf);        /// 将上面格式化好的时间字符串拼接上去
        dest->append("] [", 3);
        dest->append(1, kLogLevelStr[static_cast<int>(msg.loglevel_)]);     /// 找到日志级别索引下标添加日志
        dest->append("] [", 3);
        dest->append(msg.location_.file_name_.data(), msg.location_.file_name_.size());   /// 处理文件源位置
        dest->append(":", 1);
        dest->append(std::to_string(msg.location_.line_));      /// 文件行数
        dest->append("] [", 3);
        dest->append(std::to_string(GetProcessId()));           /// 进程号
        dest->append(":", 1);
        dest->append(std::to_string(GetThreadId()));            /// 线程号
        dest->append("] ", 2);
        dest->append(msg.message.data(), msg.message.size());      /// 最后的日志信息主体
    }
}
