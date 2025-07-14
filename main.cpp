#include <iostream>

#include "third_party/fmt/include/fmt/core.h"
int main() {
    // 测试 fmt 的格式化字符串功能
    std::string formatted_str = fmt::format("Hello, {}!", "World");
    std::cout << "Formatted string: " << formatted_str << std::endl;
    // 测试 fmt 的打印功能
    fmt::print("This is a test message from fmt.\n");
    fmt::print("The answer is {}.\n", 42);
    return 0;
}