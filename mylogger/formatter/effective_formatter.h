/**
 * @file      effective_formatter.h
 * @brief     [日志格式化器]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "formatter/formatter.h"

namespace logger {
    class EffectiveFormatter : public Formatter {
    public:
        void Format(const LogMsg& msg, MemoryBuf* dest) override;
    };
}