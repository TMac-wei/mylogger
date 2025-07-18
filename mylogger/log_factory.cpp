/**
 * @file      log_factory.cpp
 * @brief     [工厂模式具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "log_factory.h"

namespace logger {
    /**
     * @brief 私有构造函数
     */
    LogFactory::LogFactory() {}

    /**
     * @brief 获取单例
     * @return  静态工厂单例
     */
    LogFactory &LogFactory::Instance() {
        static LogFactory instance;
        return instance;
    }

    /**
     * @brief  获取VariadicLogHandle指针
     * @return
     */
    VariadicLogHandle *LogFactory::GetLogHandle() {
        /// 使用get（）获取智能指针内部管理的对象指针
        return log_handle_.get();
    }

    /**
     * @brief 设置日志处理去
     * @param log_handle
     */
    void LogFactory::SetLogHandle(std::shared_ptr<VariadicLogHandle> log_handle) {
        log_handle_ = log_handle;
    }
}