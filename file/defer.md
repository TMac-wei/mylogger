# 使用 RAII 实现 C++ 的 defer 机制

## 简介

在 C++ 中，资源管理（如内存、文件句柄、锁等）是一项关键任务。现代 C++ 提倡使用 **RAII（资源获取即初始化）** 方式来自动管理资源生命周期，避免手动释放导致的资源泄漏、异常不安全等问题。

本项目中实现了一个通用的 **defer 机制**，类似于 Go 中的 `defer`，用于在作用域退出时自动执行某段代码，以提升代码的安全性与可读性。

---

## 核心思想：RAII（Resource Acquisition Is Initialization）

**RAII** 是 C++ 的一个编程习惯，其核心是将资源的生命周期绑定到对象生命周期上：

* **获取资源 → 构造对象**
* **释放资源 → 析构对象**

一旦对象离开作用域，其析构函数就会被调用，自动清理资源。

这意味着：

```cpp
{
    std::lock_guard<std::mutex> lock(mtx); // 构造时加锁
    // 临界区代码
} // 自动释放锁（调用析构）
```

同样地，在项目中可以借助 RAII 实现 `defer` 逻辑。

---

## 实现原理：ExecuteOnScopeExit

### 类定义

```cpp
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
        if (func_) func_();
    }

private:
    std::function<void()> func_;
};
```

### 核心逻辑

* 构造时绑定任意可调用对象（函数、lambda）
* 析构时调用它
* 禁止拷贝，支持移动（防止副本干扰作用域生命周期）

---

## 宏定义实现 defer

C++ 没有内建的 `defer`，需要使用宏封装自动构造匿名对象：

```cpp
#define _LOG_CONCAT(a, b) a##b
#define _MAKE_DEFER_(line) logger::ExecuteOnScopeExit _LOG_CONCAT(defer, line) = [&]()
#define LOG_DEFER _MAKE_DEFER_(__LINE__)
```

### 宏展开示例

```cpp
LOG_DEFER {
    std::cout << "defer: clean up!\n";
};
```

相当于：

```cpp
logger::ExecuteOnScopeExit defer42 = [&]() {
    std::cout << "defer: clean up!\n";
};
```

---

## 使用示例

### 基本使用

```cpp
void example() {
    LOG_DEFER {
        /// 在离开整个作用域时调用
        std::cout << "scope exited!\n";
    };

    std::cout << "inside scope\n";
}
```

输出：

```
inside scope
scope exited!
```

### 多个 defer（后进先出 LIFO）

```cpp
LOG_DEFER { std::cout << "3\n"; };
LOG_DEFER { std::cout << "2\n"; };
LOG_DEFER { std::cout << "1\n"; };
```

输出：

```
1
2
3
```

---

##  异常安全保证

```cpp
void test() {
    LOG_DEFER {
        std::cout << "cleanup even if exception\n";
    };

    throw std::runtime_error("error");
}
```

即使发生异常，`defer` 注册的清理操作也会执行。

---

## 注意事项

* 宏 `LOG_DEFER` 生成的变量名依赖 `__LINE__`，每个 `defer` 必须单独一行声明
* `defer` 语句后必须以 `;` 结尾
* 作用范围限定在当前作用域，不能跨函数、跨语句块

---

## 与其他语言的 defer 对比

| 语言     | 关键字      | 特性               |
| ------ | -------- | ---------------- |
| Go     | defer    | 编译器内建            |
| Rust   | Drop     | Trait-based RAII |
| C++    | RAII + 宏 | 本项目方式            |
| Python | with     | 上下文管理器           |

---

## 总结

通过 RAII 和宏组合，C++ 可以优雅地模拟 `defer` 机制，自动清理资源、提升异常安全性，并保持代码简洁清晰。

这种模式特别适用于：

* 文件句柄、锁、线程等资源自动释放
* 调试和性能分析钩子
* 保证函数返回前执行特定操作

---

如需了解具体用法或加入到你的项目中，请参考测试文件 `defer_test.cpp`。

