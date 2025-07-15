cmake_minimum_required(VERSION 3.10)
project(GoogleTest)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加测试可执行文件
add_executable(google_test
        defer_test.cpp
        space_test.cpp
        thread_pool.cpp
        strand_executor.cpp
        context.cpp
        log_handle_test.cpp
        zlib_compress_test.cpp)

# 设置头文件搜索路径
target_include_directories(google_test PRIVATE
        ${PROJECT_SOURCE_DIR}/..       #根目录
        ${PROJECT_SOURCE_DIR}/../mylogger
        ${PROJECT_SOURCE_DIR}/../mylogger/third_party/googletest/googletest/include)


# 链接 自己编译的 GTest
target_link_libraries(google_test PRIVATE
        mylogger_lib
        gtest
        gtest_main
        zlib
)

enable_testing()
add_test(NAME google_test COMMAND google_test)

if(TARGET zlib)
    message(STATUS "zlib 目标存在，已链接到 google_test")
else()
    message(WARNING "zlib 目标不存在，链接失败！")
endif()

message(STATUS "Targets:")
message(STATUS "  mylogger_lib: $<TARGET_FILE:mylogger_lib>")
message(STATUS "  gtest: $<TARGET_FILE:gtest>")
message(STATUS "  gtest_main: $<TARGET_FILE:gtest_main>")
