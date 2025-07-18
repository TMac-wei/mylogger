/**
 * @file      log_factory.h
 * @brief     [工厂模式]
 * @author    Weijh
 * @version   1.0
 *       采用单例模式设计，用于管理和提供VariadicLogHandle实例
 *       工厂模式解耦合日志处理器的创建和使用，支持运行时动态切换实现
 *       保证线程安全（C++11起静态局部变量初始化是线程安全的，Instance()无需额外同步）
 *       使用智能指针管理，避免内存泄漏
 */

#pragma once

#include <memory>

namespace logger {
    class VariadicLogHandle;            /// 前向声明

    /// 工厂类
    class LogFactory {
    public:
        LogFactory(const LogFactory&) = delete;
        LogFactory& operator=(const LogFactory&) = delete;

        /// 静态获取单例的引用
        static LogFactory& Instance();

        /// 获取日志处理器
        VariadicLogHandle* GetLogHandle();

        void SetLogHandle(std::shared_ptr<VariadicLogHandle> log_handle);
    private:
        /// 单例模式写法，构造函数私有化
        LogFactory();

        std::shared_ptr<VariadicLogHandle> log_handle_;     /// 利用智能指针管理日志格式化和过滤
    };
}
