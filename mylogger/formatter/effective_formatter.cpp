/**
 * @file      effective_formatter.cpp
 * @brief     [日志格式化器具体实现--->将日志消息转换为protobuf格式]
 * @author    Weijh
 * @version   1.0
 */

#include "formatter/effective_formatter.h"
#include "internal_log.h"

#include <chrono>

#include "proto/effective_msg.pb.h"
#include "utils/sys_util.h"

namespace logger {
    /**
     *
     * @param msg    日志消息结构
     * @param dest   输出缓冲区，存储序列化之后的Protobuf日志信息
     *
     * 将格式化的日志消息转换为ProtoBuf格式的EffectiveMsg格式，便于在网络传输或者存储时保存日志的完整格式
     * 收集日志的级别、PID、TID、loc、日志内容等信息进行序列化
     * 实现基类Formatter中format函数的重写
     */
    void EffectiveFormatter::Format(const logger::LogMsg &msg, logger::MemoryBuf *dest) {
        /// 构建Protobuf消息
        EffectiveMsg effective_msg;

        /// 收集对应的信息  收集的顺序与proto中定义的顺序一致
        effective_msg.set_level(static_cast<int>(msg.loglevel_));
        /// 获取时间戳
        effective_msg.set_timestamp(
                std::chrono::duration_cast<std::chrono::microseconds>
                        (std::chrono::system_clock::now().time_since_epoch()).count());
        effective_msg.set_pid(GetProcessId());
        effective_msg.set_tid(GetThreadId());
        effective_msg.set_line(msg.location_.line_);
        effective_msg.set_file_name(msg.location_.file_name_.data(), msg.location_.file_name_.size());
        effective_msg.set_func_name(msg.location_.func_name_.data(), msg.location_.func_name_.size());
        effective_msg.set_log_info(msg.message.data(), msg.message.size());

        /// 进行序列化
        /// 计算 Protobuf 消息的二进制大小
        size_t len = effective_msg.ByteSizeLong();
        /// 调整输出缓冲区大小
        dest->resize(len);
        /// 将消息序列化到缓冲区
        bool serialize_success = effective_msg.SerializeToArray(dest->data(), len);
        if (!serialize_success) {
            /// 首先检查message，防止空指针
            std::string msg_safe = msg.message.empty() ? "[Empty Message]" : std::string(msg.message);
            /// 序列化失败，输出
            LOG_ERROR("Protobuf serialization failed! Details: [Size={} bytes], [Content='{}']",
                      len, msg_safe);
            /// 将报错信息写入到输出缓冲区
            dest->assign("[Serialization Error]", 20);
        }
    }
}