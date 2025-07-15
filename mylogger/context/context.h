/**
 * @file      context.h
 * @brief     [日志系统调度器，Executor的进一步封装，以提供给用户使用]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "context/executor.h"

#include <memory>

namespace logger {
    namespace ctx {
        /// 通过pimpl模式进行封装，便于拓展和保证数据私有部分安全
        class ExecutorManager; /// 负责管理任务执行器资源

        class Context {
        public:
            /// 返回执行器指针，用于任务调度
            Executor *GetExecutor() const;

            /// 单例模式获取Context,使用局部对象的方式管理单例，而不是指针
            static Context &GetInstance() {
                static Context instance;
                return instance;
            }

            /// 创建新的任务运行器并返回标签
            TaskRunnerTag NewTaskRunner(TaskRunnerTag tag);

            /// 禁止拷贝和复制
            Context(const Context &) = delete;

            Context &operator=(const Context &) = delete;

            /// 移动构造和赋值
            Context(Context &&) = delete;

            Context &operator=(Context &&) = delete;

            ~Context();

        private:
            /// 单例模式的条件，构造函数私有化，提供静态方法获取实例
            Context();


            std::unique_ptr<ExecutorManager> executor_manager_;         /// 任务执行管理器的外部接口
        };
    }
}

/// 提供宏定义，更方便的调用异步任务执行管理器,同时使用内联函数代替复杂宏定义
/// 获取CONTEXT单例
#define CONTEXT logger::ctx::Context::GetInstance()

/// 获取任务执行管理器
#define EXECUTOR CONTEXT.GetExecutor()

/// 获取新的任务标识
#define NEW_TASK_RUNNER(tag) CONTEXT.NewTaskRunner(tag)

/// 任务调度
#define POST_TASK(runner_tag, task) EXECUTOR->PostTask(runner_tag, task)

/// 阻塞等待某个runner执行器执行完毕所有任务
inline void WAIT_TASK_IDLE(logger::ctx::TaskRunnerTag runner_tag) {
    EXECUTOR->PostTaskAndGetResult(runner_tag, [] {})->wait();
}

/// 执行周期任务
template<typename Task, typename Rep, typename Period>
inline void POST_REPEATED_TASK(
        logger::ctx::TaskRunnerTag runner_tag,
        Task &&task,
        const std::chrono::duration<Rep, Period> &delta,
        size_t repeat_num
) {
    EXECUTOR->PostRepeatedTask(runner_tag,
                               std::forward<Task>(task),
                               delta,
                               repeat_num);
}

///// 阻塞等待某个runner执行器执行完毕所有任务
//#define WAIT_TASK_IDLE(runner_tag) EXECUTOR->PostTaskAndGetResult(runner_tag, [](){}) -> wait()
//
///// 执行周期性任务
//#define POST_REPEATED_TASK(runner_tag, task, delta, repeat_num) \
//    EXECUTOR->PostRepeatedTask(runner_tag, task, delta, repeat_num)
