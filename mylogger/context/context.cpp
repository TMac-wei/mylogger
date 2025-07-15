/**
 * @file      context.cpp
 * @brief     [异步任务执行管理器实现]
 * @author    Weijh
 * @version   1.0
 */

#include "context/context.h"

namespace logger {
    namespace ctx {
        /// 实现ExecutorManager
        class ExecutorManager {
        public:
            ExecutorManager() {
                /// 初始化任务调度器
                executor_ = std::make_unique<Executor>();
            }

            ~ExecutorManager() {
                /// 释放资源--> Context中使用unique_ptr管理ExecutorMange，会自动管理资源，这里也可以显式调用reset()
                /// executor_.reset();
            }

            Executor *GetExecutor() {
                return executor_.get();
            }

            TaskRunnerTag NewTaskRunner(TaskRunnerTag tag) {
                return executor_->AddTaskRunner(tag);
            }

        private:
            std::unique_ptr<Executor> executor_;
        };

        /// Context通过调用内部的ExecutorManager来管理内部的异步任务执行器
        Context::Context() : executor_manager_(std::make_unique<ExecutorManager>()) {}

        /// 实现Context的析构函数
        Context::~Context() = default;

        Executor *Context::GetExecutor() const {
            return executor_manager_->GetExecutor();
        }

        TaskRunnerTag Context::NewTaskRunner(TaskRunnerTag tag) {
            return executor_manager_->NewTaskRunner(std::move(tag));
        }
    }
}