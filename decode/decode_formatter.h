/**
 * @file      decode_formatter.h
 * @brief     [日志解析格式化]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "mylogger/proto/effective_msg.pb.h"

#include <string>

namespace logger {

    /**
     * @brief DecodeFormatter 解析格式化抽象基类
     */
    class DecodeFormatter {
    public:
        /// 设置日志解析的格式
        void SetPattern(const std::string &pattern);

        /// 格式化器
        void Format(const EffectiveMsg &msg, std::string &dest);

    };

}
