/**
 * @file      executor.h
 * @brief     [strand机制延时任务执行器]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "context/thread_pool.h"

namespace logger {
    namespace ctx {

        /// 类型别名定义
        using Task = std::function<void(void)>;
        using TaskRunnerTag = uint64_t;                 /// 任务执行器的唯一标识
        using RepeatedTaskId = uint64_t;                /// 周期性任务的唯一标识

        /// 任务执行器类中包含，任务执行器上下文和任务执行定时器两个内部类
        class Executor {

            /// 首先定义任务执行定时器与上下文内部类
        private:
            /**
             * ExecutorContext 是 Executor 类的内部嵌套类，
             * 主要负责任务执行器（TaskRunner）的管理->每个任务执行器本质上是一个单线程的 ThreadPool（线程池）
             * 通过 TaskRunnerTag（唯一标识）区分不同的任务执行器，实现任务的隔离和串行执行（同一执行器的任务按提交顺序执行）。
             * 包括任务执行器的创建、标识分配和查询等功能。它是 Executor 组件中管理任务执行载体的核心部分。
             * */
            class ExecutorContext {
            public:
                /// 构造和析构默认，禁止拷贝和赋值
                ExecutorContext() = default;

                ~ExecutorContext() = default;

                ExecutorContext(const ExecutorContext &) = delete;

                ExecutorContext &operator=(const ExecutorContext &) = delete;

                /// 添加任务执行器
                TaskRunnerTag AddTaskRunner(const TaskRunnerTag &tag);

            private:
                using TaskRunner = ThreadPool;                          /// 每一个线程池就是一个任务执行器
                using TaskRunnerPtr = std::unique_ptr<TaskRunner>;      /// 每个任务执行器（线程池中）只有一个线程，所以使用unique_ptr
                friend class Executor;                                  /// 保证外部类可以访问，任务执行管理类的私有成员

                /// 获取任务执行器
                TaskRunner *GetTaskRunner(const TaskRunnerTag &tag);

                /// 获取任务执行标志
                TaskRunnerTag GetNextRunnerTag();

                /// 存储TaskRunnerTag与任务执行器（ThreadPool）的映射关系，键为唯一标识，值为智能指针管理的线程池
                std::unordered_map<TaskRunnerTag, TaskRunnerPtr> task_runner_dict_;
                /// 互斥锁，保护 task_runner_dict_ 的增删查操作，确保多线程环境下的线程安全
                std::mutex mutex_;
            };

            /**
             * ExecutorTimer 是 Executor 组件中的定时器实现，负责管理延迟任务和周期性任务的调度。
             * 它通过优先级队列和条件变量实现高效的任务排序和超时等待，是整个组件的时间控制核心。
             * */
            class ExecutorTimer {
            public:
                /// 内部任务结构体：存储任务执行时间、任务函数和重复任务ID
                struct Internals {
                    std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
                    Task task;
                    RepeatedTaskId repeatedTaskId;

                    /// 重载比较运算符，按照执行时间排序（时间小的优先）
                    bool operator<(const Internals &other) const {
                        return time_point > other.time_point;
                    }
                };

                /// 构造与析构
                ExecutorTimer();

                ~ExecutorTimer();

                ExecutorTimer(const ExecutorTimer &) = delete;

                ExecutorTimer &operator=(const ExecutorTimer &) = delete;

                /// 接口函数
                bool Start();

                void Stop();

                /// 推送延迟队列
                void PostDelayedTask(Task task, const std::chrono::microseconds &delta);

                /// 推动周期任务
                RepeatedTaskId PostRepeatedTask(Task task, const std::chrono::microseconds &delta, uint64_t repeat_num);

                /// 取消任务
                void CancelRepeatedTask(RepeatedTaskId task_id);

            private:
                /// 定时器主循环
                void Run_();

                /// 周期性任务调度
                void PostRepeatedTask_(Task task,
                                       const std::chrono::microseconds &delta,
                                       RepeatedTaskId repeated_task_id,
                                       uint64_t repeat_num);

                /// 推送普通任务
                void PostTask_(Task task,
                               std::chrono::microseconds &delta,
                               RepeatedTaskId repeated_task_id,
                               uint64_t repeat_num);

                /// 获取周期性任务的ID
                RepeatedTaskId GetRepeatedTaskId();

                /// 私有成员变量
                /// 存储待执行的延迟任务和周期性任务，确保每次取出的任务是最接近执行时间的任务
                std::priority_queue<Internals> queue_;
                std::mutex mutex_;                           /// 保护任务队列 queue_ 的互斥锁
                std::condition_variable cond_;              /// 条件变量
                std::atomic<bool> running_;                 /// 运行状态
                std::unique_ptr<ThreadPool> thread_pool_;   /// 线程池
                std::atomic<RepeatedTaskId> repeated_task_id_;  /// 周期任务的唯一ID
                /**
                 * repeated_id_state_set_：存储活跃的周期性任务 ID
                 *      任务提交时将 ID 加入集合
                 *      任务取消时从集合中移除ID
                 *      执行任务前检查ID是否存在，确保被取消的任务不再执行
                 * */
                std::unordered_set<RepeatedTaskId> repeated_id_state_set_;

            };


        public:
            Executor();     /// 初始化内部组件（ExecutorContext 和 ExecutorTimer）
            ~Executor();    /// 释放内部组件（ExecutorContext 和 ExecutorTimer）的资源

            /// 禁止拷贝和赋值
            Executor(const Executor &) = delete;

            Executor &operator=(const Executor &) = delete;

            /// 任务执行器管理
            TaskRunnerTag AddTaskRunner(const TaskRunnerTag &tag);

            /// 任务提交--> 向指定的任务执行器提交即时任务
            void PostTask(const TaskRunnerTag &runner_Tag, Task task);

            /// 延迟任务提交--模板
            template<typename R, typename P>
            void PostDelaydTask(const TaskRunnerTag &runner_tag, Task task,
                                const std::chrono::duration<R, P> &delta) {
                Task func = std::bind(&Executor::PostTask, this, runner_tag, std::move(task));

                executor_timer_->Start();
                executor_timer_->PostDelayedTask(std::move(func),
                                                 std::chrono::duration_cast<std::chrono::microseconds>(delta));
            }

            /// 周期任务提交
            template<typename R, typename P>
            RepeatedTaskId PostRepeatedTask(const TaskRunnerTag &runnerTag,
                                            Task task,
                                            const std::chrono::duration<R, P> &delta,
                                            uint64_t repeat_num) {

            }

            /// 带有返回值的任务提交
            template<typename F, typename... Args>
            auto PostTaskAndGetResult(const TaskRunnerTag &runnerTag,
                                      F &&func,
                                      Args &&... args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>> {

            }

            /// 取消任务
            void CancelRepeatedTask(RepeatedTaskId taskId);


        private:
            /// Executor的成员变量
            std::unique_ptr<ExecutorContext> executor_context_;     ///
            std::unique_ptr<ExecutorTimer> executor_timer_;         ///











        };

    }
}