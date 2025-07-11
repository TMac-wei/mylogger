cmake_minimum_required(VERSION 3.10)
project(myLogger)

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VCPKG 工具链路径（用于查找 GTest 等包）  这里使用gtest会发生冲突，所以使用手动链接源码的方式
#set(CMAKE_TOOLCHAIN_FILE "D:/c++_projects/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake"
#        CACHE STRING "Vcpkg toolchain file")

# 启用测试
enable_testing()

# 添加子目录(子目录有自己的 CMakeLists.txt）
add_subdirectory(mylogger)
add_subdirectory(mylogger/third_party/googletest)
add_subdirectory(GoogleTest)

# 主可执行程序
add_executable(mylogger main.cpp)

# 添加头文件目录
target_include_directories(mylogger
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger
)

target_link_libraries(mylogger PRIVATE mylogger_lib)

## ----------------- Google Test 区域 -----------------
## 启用测试
#enable_testing()
#
## 添加 GTest 测试可执行文件
#add_executable(main_google_test
#        GoogleTest/v.cpp
#)
#
## 链接 GTest
#target_link_libraries(main_google_test
#        PRIVATE
#        GTest::gtest
#        GTest::gtest_main
#)
#
## 添加 include 路径
#target_include_directories(main_google_test
#        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger
#)
#
## 注册测试
#add_test(NAME Main_Google_Test COMMAND main_google_test)
#
