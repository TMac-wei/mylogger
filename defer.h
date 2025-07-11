/**
 * @file      defer.h
 * @brief     [延迟执行函数，保证内存释放，避免内存泄漏，实现RAII思想]
 * @author    Weijh
 * @version   1.0
 */

#if 0
#pragma once

#include <concepts>
#include <utility>

namespace logger {

    template <typename F>
    requires std::invocable<F>
    class ExecuteOnScopeExit {
    public:
        explicit ExecuteOnScopeExit(F&& func) noexcept
                : func_(std::move(func)) {}

        /// 移动拷贝，使用std::exchange()安全的交换资源
        ExecuteOnScopeExit(ExecuteOnScopeExit&& other) noexcept
            : func_(std::exchange(other.func_, nullptr)) {}

        /// 移动赋值运算符
        ExecuteOnScopeExit& operator=(ExecuteOnScopeExit&& other) noexcept {
            if (this != &other) {
                func_ = std::exchange(other.func_, nullptr);
            }
            return *this;
        }

        /// 禁止拷贝构造和赋值，确保对象只执行一次
        ExecuteOnScopeExit(const ExecuteOnScopeExit&) = delete;
        ExecuteOnScopeExit& operator=(const ExecuteOnScopeExit&) = delete;

        /// 析构函数，在离开作用域时，自动执行func_
        ~ExecuteOnScopeExit() noexcept {
            if (func_) {
                func_();
            }
        }

    private:
        F func_;        /// 退出作用域执行的操作
    };

    /// 类型推导，实现 ExecuteOnScopeExit(F) -> ExecuteOnScopeExit<F>
    /// 告诉编译器：当用类型为 F 的参数构造 ExecuteOnScopeExit 时，推导出的模板参数就是 F
    /// 虽然 ExecuteOnScopeExit 的构造函数模板参数可以接受任意类型 F，但编译器默认只能推导函数模板的参数，而不是类模板的参数。推导指引的存在是为了：
    /// 显式告诉编译器如何进行推导
    /// 支持更复杂的推导场景（例如从多个构造函数参数推导出不同的模板参数）
    /// 允许用户在创建 ExecuteOnScopeExit 对象时省略模板参数
    template <typename F>
    ExecuteOnScopeExit(F) -> ExecuteOnScopeExit<F>;

}

/// 宏定义
#define _LOG_CONCAT(a, b) a##b
#define _MAKE_DEFER_(line) logger::ExecuteOnScopeExit _LOG_CONCAT(defer, line) = [&]()

#undef LOG_DEFER
#define LOG_DEFER _MAKE_DEFER_(__LINE__)

#else

#pragma once

#include <functional>

namespace logger {

    class ExecuteOnScopeExit {
    public:
        ExecuteOnScopeExit() = default;

        ExecuteOnScopeExit(ExecuteOnScopeExit&&) = default;
        ExecuteOnScopeExit& operator=(ExecuteOnScopeExit&&) = default;

        ExecuteOnScopeExit(const ExecuteOnScopeExit&) = delete;
        ExecuteOnScopeExit& operator=(const ExecuteOnScopeExit&) = delete;

        template <typename F, typename... Args>
        ExecuteOnScopeExit(F&& f, Args&&... args) {
            func_ = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        }

        ~ExecuteOnScopeExit() noexcept {
            if (func_) {
                func_();
            }
        }

    private:
        std::function<void()> func_;
    };
}  // namespace logger

#define _LOG_CONCAT(a, b) a##b
#define _MAKE_DEFER_(line) logger::ExecuteOnScopeExit _LOG_CONCAT(defer, line) = [&]()

#undef LOG_DEFER
#define LOG_DEFER _MAKE_DEFER_(__LINE__)

#endif