/**
 * @file      thread_pool.h
 * @brief     [线程池实现-支持普通任务和带返回值的任务]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace logger {
    namespace ctx {

        /// 构建线程池，需要支持不带返回值的普通函数以及带返回值的函数
        class ThreadPool {
        public:
            /// 显式单参构造函数，避免隐式转换
            explicit ThreadPool(uint32_t thread_count)
                    : thread_count_(thread_count), is_shutdown_(false), is_available_(false) {}

            /// 禁止拷贝和赋值
            ThreadPool(const ThreadPool &) = delete;

            ThreadPool &operator=(const ThreadPool &) = delete;

            ~ThreadPool() {
                Stop();         /// 停止所有线程，并且进行通知
            }

            /// 线程池的启动和停止接口
            bool Start();

            void Stop();

            /// 提交不带返回值的普通任务
            template<typename F, typename... Args>
            void SubmitTask(F &&func, Args &&... args) {
                /// 终止条件判断
                if (is_shutdown_.load() || !is_available_.load()) {
                    return;
                }

                /// 绑定任务
                auto task = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
                {
                    /// 获取任务
                    std::lock_guard<std::mutex> lock(task_mutex_);
                    tasks_.emplace([task]() {
                        /// 将需要执行的任务添加到任务队列
                        task();
                    });
                }

                /// 通知线程，执行任务,提交一个线程只需要唤醒一个线程，避免惊群效应
                task_cv_.notify_one();
            }

            /// 提交带返回值的任务--> 使用auto类型自动推导以及invoke_对结果进行打包
            template<typename F, typename...Args>
            auto SubmitRetTask(F &&func, Args...args) -> std::future<std::invoke_result_t<F, Args...>> {
                /// 定义返回类型别名
                using ret_type = std::invoke_result_t<F, Args...>;
                if (is_shutdown_.load() || !is_available_.load()) {
                    /// 这里应该返回一个空的future对象
                    return std::future<ret_type>();
                }

                auto bind_task = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
                /// 打包获取的带返回值的任务，并且使用智能指针进行管理
                auto task = std::make_shared<std::packaged_task<ret_type>()>(std::move(bind_task));
                std::future<ret_type> res = task->get_future();
                {
                    std::unique_lock<std::mutex> lock(task_mutex_);
                    tasks_.emplace([task]() {
                        (*task)();  /// 调用任务
                    });
                }
                task_cv_.notify_one();
                return res;
            }

        private:
            /// 成员变量
            /// 增加线程函数
            void AddThread();

            /// 通过智能指针来进行管理线程
            using ThreadPtr = std::shared_ptr<std::thread>;
            using Task = std::function<void()>;     /// 任务，每一个任务就是需要执行的函数

            /// 线程信息的结构体
            struct ThreadInfo {
                ThreadInfo() = default;

                ~ThreadInfo();

                ThreadPtr ptr{nullptr};
            };

            /// 负责保存线程信息的智能指针
            using ThreadInfoPtr = std::shared_ptr<ThreadInfo>;

            std::vector<ThreadInfoPtr> work_threads_;       /// 工作线程数组
            std::queue<Task> tasks_;                        /// 任务队列
            std::mutex task_mutex_;                         /// 任务锁
            std::condition_variable task_cv_;               /// 条件变量，用于阻塞线程等待任务

            /// 使用原子变量处理基本的线程数量以及状态信息
            std::atomic<uint32_t> thread_count_;            /// 可设定的线程池中线程数量
            std::atomic<bool> is_shutdown_;                 /// 线程是否关闭
            std::atomic<bool> is_available_;                /// 线程池是否还有效
        };
    }
}
