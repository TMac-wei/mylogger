/**
 * @file      formatter.h
 * @brief     [格式器抽象类--通用接口]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_msg.h"
#include "log_commom.h"

namespace logger {
    class Formatter {
    public:
        virtual ~Formatter() = default;

        virtual void Format(const LogMsg &msg, MemoryBuf *dest) = 0;
    };
}
