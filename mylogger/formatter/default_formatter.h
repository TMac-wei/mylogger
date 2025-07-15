/**
 * @file      default_formatter.h
 * @brief     [基础的默认格式化器，重写formatter中的Format方法]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "formatter/formatter.h"

namespace logger {

    /// 默认的格式化器，继承自formatter这个抽象基类接口
    class DefaultFormatter : public Formatter {
    public:
        /// 重写抽象类中的Format方法
        void Format(const LogMsg &msg, MemoryBuf *dest) override;
    };
}
