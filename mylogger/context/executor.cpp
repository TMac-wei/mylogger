/**
 * @file      executor.cpp
 * @brief     [任务执行器的具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "context/executor.h"

namespace logger {
    namespace ctx {
        /***
         * @brief 初始化ExecutorTimer和ExecutorTimer
         * */
        Executor::Executor() {
            executor_timer_ = std::make_unique<ExecutorTimer>();
            executor_context_ = std::make_unique<ExecutorContext>();
        }

        /***
         * @brief 析构函数，调用reset()释放资源
         * */
        Executor::~Executor() {
            executor_context_.reset();
            executor_timer_.reset();
        }

        /***
         * @brief 添加一个任务执行器
         * @param const TaskRunnerTag& tag-->用户指定的标识，若该标识已被占用，则自动生成新的唯一标识
         *        调用底层ExecutorContext里的AddTaskRunner(),作为外部接口的封装
         * */
        TaskRunnerTag Executor::AddTaskRunner(const logger::ctx::TaskRunnerTag &tag) {
            return executor_context_->AddTaskRunner(tag);
        }

        /***
         * @brief 传递一个普通任务
         * @param const TaskRunnerTag& tag-->用户指定的标识
         *        Task task
         * */
        void Executor::PostTask(const logger::ctx::TaskRunnerTag &runner_Tag, logger::ctx::Task task) {
            /// 从ExecutorContext获取一个任务执行器，通过任务执行器去提交普通任务
            ExecutorContext::TaskRunner *task_runner = executor_context_->GetTaskRunner(runner_Tag);
            task_runner->SubmitTask(std::move(task));
        }

        /***
         * @brief 取消周期任务
         * @param RepeatedTaskId task_id-->周期任务的ID
         * @return 无
         *          调用底层定时器的取消周期任务接口，将这个接口作为最外层接口对外部开发
         * */
        void Executor::CancelRepeatedTask(logger::ctx::RepeatedTaskId taskId) {
            executor_timer_->CancelRepeatedTask(taskId);
        }

        /***
         * @brief 创建一个新的任务执行器，并且返回其唯一标识TaskRunnerTag
         * @param const TaskRunnerTag& tag-->用户指定的标识，若该标识已被占用，则自动生成新的唯一标识
         * @return 实际分配的唯一标识TaskRunnerTag
         * */
        TaskRunnerTag Executor::ExecutorContext::AddTaskRunner(const TaskRunnerTag &tag) {
            std::lock_guard<std::mutex> lock(mutex_);
            TaskRunnerTag latest_tag = tag;
            /// 若用户指定的tag已经被占用，则返回下一个tag
            while (task_runner_dict_.find(tag) != task_runner_dict_.end()) {
                latest_tag = GetNextRunnerTag();        /// 自动获取下一个tag
            }

            /// 创建单线程线程池作为任务执行器，并启动
            TaskRunnerPtr runner = std::make_unique<ThreadPool>(1);
            runner->Start();        /// 启动线程池（任务执行器）
            /// 存入映射表
            task_runner_dict_.emplace(latest_tag, std::move(runner));
            return latest_tag;
        }

        /***
        * @brief 获取任务执行器->根据 TaskRunnerTag 查找并返回对应的任务执行器（ThreadPool 指针）。
        * @param TaskRunnerTag
        * @return 找到则返回 ThreadPool*，否则返回 nullptr。
        * */
        Executor::ExecutorContext::TaskRunner *Executor::ExecutorContext::GetTaskRunner(
                const logger::ctx::TaskRunnerTag &tag) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = task_runner_dict_.find(tag);
            if (it == task_runner_dict_.end()) {
                return nullptr;
            }
            return it->second.get();    /// 返回智能指针管理的线程池地址
        }

        /***
       * @brief 生成一个自增的、唯一的 TaskRunnerTag，用于当用户指定的 tag 已被占用时自动分配新标识
       * @return 生成的唯一标识TaskRunnerTag
       *    通过静态变量index的自增确保标识唯一性，无需加锁（调用方AddTaskRunner已经加锁了）
       * */
        TaskRunnerTag Executor::ExecutorContext::GetNextRunnerTag() {
            static uint64_t index = 0;
            ++index;
            return index;
        }

        /***
        * @brief 构造定时器
        *       为Executor的成员变量线程池赋值，并且更新周期ID与运行状态
        * */
        Executor::ExecutorTimer::ExecutorTimer() {
            /// 创建线程池，并且初始化定时器的相关成员变量
            thread_pool_ = std::make_unique<ThreadPool>(1);
            repeated_task_id_.store(0);
            running_.store(false);
        }

        /***
        * @brief 析构，释放资源
         *      调用底层的Stop()函数释放资源
        * */
        Executor::ExecutorTimer::~ExecutorTimer() {
            Stop(); /// 调用Stop()
        }

        /***
        * @brief 定时器的核心实现，启动定时器
        * @return 是否启动成功的状态
         *      如果已经启动了，则直接返回，否则更新对应的成员状态
         *      启动线程池，并且在工作线程中绑定定时器主循环
        * */
        bool Executor::ExecutorTimer::Start() {
            if (running_) {
                return true;
            }
            /// 更新running_
            running_.store(true);
            /// 启动线程池并提交普通任务
            bool ret = thread_pool_->Start();
            thread_pool_->SubmitTask(&Executor::ExecutorTimer::Run_, this);
            return ret;
        }

        /***
        * @brief 定时器的核心实现，停止定时器
        * @return 是否启动成功的状态
         *      运行状态修改为停止
         *      唤醒所有等待的线程
         *      释放所有线程池的资源
        * */
        void Executor::ExecutorTimer::Stop() {
            running_.store(false);
            cond_.notify_all();
            thread_pool_.reset();   /// 释放线程池资源
        }

        /***
        * @brief 定时器关键函数 Run_
        * @param 无
        * @return 无
         *        所有操作都是保证在任务执行器在执行过程中实现
         *          如果任务队列为空，那么就需要一直等待任务
         *          否则取出队首的任务，计算消耗的时间，如果时间差距大于0，则等待这么长的时间间隔，然后继续继续执行
         *              如果没有足够的时间间隔，则将首元素出队，解锁，并且马上执行任务
        * */
        void Executor::ExecutorTimer::Run_() {
            while (running_.load()) {
                std::unique_lock<std::mutex> lock(mutex_);
                if (queue_.empty()) {
                    cond_.wait(lock);
                    continue;
                }

                /// 取出队列中的任务
                auto task_already_ = queue_.top();
                auto diff = task_already_.time_point -
                            std::chrono::high_resolution_clock::now();   /// 用任务内部的时间-当前的时间，如果是延迟任务，则差值会大于0
                if (std::chrono::duration_cast<std::chrono::microseconds>(diff).count() > 0) {
                    /// 说明是延迟任务
                    cond_.wait_for(lock, diff);
                    continue;
                } else {
                    /// 说明是普通任务，立马执行
                    queue_.pop();
                    lock.unlock();
                    task_already_.task();       /// 执行任务
                }
            }
        }

        /***
        * @brief 延迟任务提交
        * @param Task task, const std::chrono::microseconds& delta
        * @return 无
         *        封装一个内部任务任务结构体，计算执行所需要的时间
         *        压入待执行任务队列中
        * */
        void Executor::ExecutorTimer::PostDelayedTask(logger::ctx::Task task, const std::chrono::microseconds &delta) {
            Internals inside_tasks_;        /// 内部任务结构体
            inside_tasks_.time_point = std::chrono::high_resolution_clock::now() + delta;       /// 计算执行所需要的时间
            inside_tasks_.task = std::move(task);
            {
                std::unique_lock<std::mutex> lock(mutex_);
                queue_.push(inside_tasks_);
                cond_.notify_one();     /// 这里使用notify_one()即可，因为可以避免惊群效应，同时唤醒所有线程，但是没有竞争到资源也是一种开销
            }
        }

        /***
       * @brief 周期任务提交
       * @param Task task, const std::chrono::microseconds& delta， uint64_t repeat_num
       * @return 无
        *        封装一个内部任务任务结构体，计算执行所需要的时间
        *        压入待执行任务队列中
       * */
        RepeatedTaskId Executor::ExecutorTimer::PostRepeatedTask(logger::ctx::Task task,
                                                                 const std::chrono::microseconds &delta,
                                                                 uint64_t repeat_num) {
            /// 获取周期唯一ID
            RepeatedTaskId id = GetRepeatedTaskId();
            repeated_id_state_set_.insert(id);
            PostRepeatedTask_(std::move(task), delta, id, repeat_num);
            return id;
        }

        /***
           * @brief 取消任务提交
           * @param RepeatedTaskId task_id
           * @return 无
           *        从周期任务集合中移除取消的任务id
           * */
        void Executor::ExecutorTimer::CancelRepeatedTask(logger::ctx::RepeatedTaskId task_id) {
            repeated_id_state_set_.erase(task_id);
        }

        /***
         * @brief ExecutorTimer定时器类内部接口，提交普通任务
         * @param Task task, const std::chrono::microseconds& delta，RepeatedTaskId repeated_task_id,uint64_t repeat_num
         * @return 无
          *        封装一个内部任务任务结构体，计算执行所需要的时间
          *        压入待执行任务队列中
         * */
        void Executor::ExecutorTimer::PostTask_(logger::ctx::Task task, std::chrono::microseconds &delta,
                                                logger::ctx::RepeatedTaskId repeated_task_id, uint64_t repeat_num) {
            /// 嵌套调用提交周期任务
            PostRepeatedTask_(task, delta, repeated_task_id, repeat_num);
        }

        /***
        * @brief 获取周期任务的ID
        * @param 无
        * @return 返回周期任务的ID
         *      返回周期任务的ID，然后将其+1，确保每个周期任务的ID不相同
        * */
        RepeatedTaskId Executor::ExecutorTimer::GetRepeatedTaskId() {
            return repeated_task_id_++;
        }

        /***
        * @brief ExecutorTimer定时器类内部接口，提交周期任务，PostRepeatedTask_ 是 ExecutorTimer 类中实现周期性任务递归调度的核心内部函数。
        * @param Task task,      需要周期性执行的任务（std::function<void(void)> 类型）
         *        const std::chrono::microseconds& delta，   两次任务执行之间的时间间隔（微秒）。
         *        RepeatedTaskId repeated_task_id,  当前周期性任务的唯一 ID，用于标识任务是否被取消。
         *        uint64_t repeat_num   剩余需要执行的次数（每次递归调用会减 1，直到 0 时停止）
        * @return 无
         *        它负责执行当前周期的任务，并自动安排下一次任务的执行，直到任务被取消或达到指定的执行次数
        * */
        void Executor::ExecutorTimer::PostRepeatedTask_(logger::ctx::Task task, const std::chrono::microseconds &delta,
                                                        logger::ctx::RepeatedTaskId repeated_task_id,
                                                        uint64_t repeat_num) {
            /// 判断任务是否需要继续执行，即需要判断任务是否还在活跃集合中
            if (repeated_id_state_set_.find(repeated_task_id) == repeated_id_state_set_.end() || repeat_num == 0) {
                return;
            }

            /// 如果仍然活跃，直接执行当前的任务
            {
                std::unique_lock<std::mutex> lock(mutex_);
                task();
            }

            /// 找到下一个需要执行的任务
//            Task func = std::bind(&Executor::ExecutorTimer::PostTask_, this, std::move(task), delta, repeated_task_id,
//                                  repeat_num - 1);
            /// 使用非递归方式提交任务
            Task func = [this, task = std::move(task), delta, repeated_task_id, repeat_num]() mutable {
                this->PostRepeatedTask_(std::move(task), delta, repeated_task_id, repeat_num - 1);
            };
            /// 创建任务内部结构体
            Internals task_inside_;
            task_inside_.time_point = std::chrono::high_resolution_clock::now() + delta;
            task_inside_.repeatedTaskId = repeated_task_id;
            task_inside_.task = std::move(func);

            {
                /// 将下一个任务添加到待执行队伍队列中
                std::unique_lock<std::mutex> lock(mutex_);
                queue_.push(task_inside_);
                lock.unlock();
                cond_.notify_one();
            }
        }


    }
}