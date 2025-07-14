/**
 * @file      thread_pool.cpp
 * @brief     [thread_pool具体实现]
 * @author    Weijh
 * @version   1.0
 */

#include "context/thread_pool.h"

namespace logger {
    namespace ctx {

        /// 线程池启动接口
        bool ThreadPool::Start() {
            /// 要避免在初始化的时候线程池中没有任何其他的状态
            if (!is_available_.load()) {
                is_available_.store(true);
                uint32_t thread_count = thread_count_.load();
                /// 循环创建线程
                for (uint32_t i = 0; i < thread_count; i++) {
                    AddThread();    /// 依次添加线程
                }
                return true;
            }
            return false;
        }

        /// 线程池停止接口
        void ThreadPool::Stop() {
            if (!is_available_.load()) {
                return;
            }

            is_available_.store(false);
            is_shutdown_.store(true);
            /// 通知所有线程
            task_cv_.notify_all();

            /// 回收所有线程保证只join一次，这里确保ThreadInfo就不需要析构join了
            for (auto& thread_info_ptr : work_threads_) {
                if (thread_info_ptr && thread_info_ptr->ptr) {      /// 智能指针存在，并且指针指向的子线程也存在
                    std::thread& t = *thread_info_ptr->ptr;         /// 获取线程对象
                    if (t.joinable()) {
                        t.join();       /// 在这里如果解引用之后的线程对象可以join，那就join
                    }
                }
            }

            /// 清空线程池容器
            work_threads_.clear();
            /// 线程池关闭的时候还需要清空所有任务队列--->将任务队列中的任务都pop()出来
            {
                std::lock_guard<std::mutex> lock(task_mutex_);
                while (!tasks_.empty()) {
                    tasks_.pop();
                }
            }
        }

        /// 增加线程函数，线程将持续从任务队列中取出任务并执行
        /**
         * AddThread():
          ├── 创建 ThreadInfoPtr 封装线程
          ├── 定义线程函数 func:
          │     ├── 无限循环
          │     ├── 加锁 + 条件变量等待任务或关闭
          │     ├── 取出任务并执行
          ├── 启动线程执行 func
          └── 添加到线程容器 worker_threads_
         * */
        void ThreadPool::AddThread() {
            /// 创建线程封装对象，创建一个ThreadInfo对象，未来将对象std::thread赋值给它
            ThreadInfoPtr thread_ptr = std::make_shared<ThreadInfo>();
            auto func = [this]() {
                /// 每个线程都会无限循环的从队列中取出任务执行
                while (true) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(task_mutex_);
                        /// 等待任务或者关闭信号，使用Lambda可以避免虚假唤醒
                        task_cv_.wait(lock, [this] {
                            return is_shutdown_ || !tasks_.empty();
                        });
                        /// 若已经关闭且任务队列为空，退出
                        if (is_shutdown_ && tasks_.empty()) {
                            break;
                        }
                        /// 队列非空，取出任务
                        if (!tasks_.empty()) {
                            /// 取出任务队列中的头元素
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        } else {
                            continue;       /// 任务队列为空，持续等待
                        }
                    }
                    task();     /// 执行任务
                }
            };

            /// 创建线程，并加入线程池
            thread_ptr->ptr = std::make_shared<std::thread>(std::move(func));
            work_threads_.emplace_back(std::move(thread_ptr));
        }

        /// 在线程池销毁阶段，所有的线程都应该确保被执行完毕，并被正确回收
        ThreadPool::ThreadInfo::~ThreadInfo() {
            /// 所有线程对象在Stop时已经被join，所以这里不需要了，否则可能会造成重复join的情况
        }
    }
}