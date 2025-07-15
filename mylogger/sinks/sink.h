/**
 * @file      sink.h
 * @brief     [分发目的地抽象类]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "log_msg.h"
#include "log_commom.h"
#include "formatter/formatter.h"

#include <memory>

namespace logger {
    class LogSink {
    public:
        virtual ~LogSink() = default;

        virtual void Log(const LogMsg& msg) = 0;

        virtual void SetFormatter(std::unique_ptr<Formatter> formatter) = 0;

        virtual void Flush(){}
    };
}
