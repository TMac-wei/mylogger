cmake_policy(SET CMP0079 NEW)  # 支持跨目录链接政策

cmake_minimum_required(VERSION 3.10)
project(myLogger)

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试
enable_testing()

# --- 第三方库配置 ---
add_subdirectory(mylogger/third_party/zlib)

# 添加 Zstd 库路径和头文件
set(ZSTD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/third_party/zstd/lib)
set(ZSTD_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/third_party/zstd/build/cmake/build_mingw/libzstd.a)


add_subdirectory(mylogger/third_party/cryptopp_cmake)
add_subdirectory(mylogger/third_party/googletest)
add_subdirectory(mylogger/third_party/fmt)


# 添加子目录(子目录有自己的 CMakeLists.txt）
add_subdirectory(mylogger)
add_subdirectory(decode)
add_subdirectory(GoogleTest)

# 定义Protobuf路径
set(PROTOBUF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/third_party/protobuf-21.8)
set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_ROOT}/include)
set(PROTOBUF_LIB_DIR ${PROTOBUF_ROOT}/lib)
set(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_ROOT}/bin/protoc.exe)


# 主可执行程序
add_executable(mylogger main.cpp)

# 添加头文件目录
target_include_directories(mylogger
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger
        ${ZSTD_INCLUDE_DIR}
        ${PROTOBUF_INCLUDE_DIR}
)

target_link_libraries(mylogger PRIVATE
        mylogger_lib
        fmt::fmt
        ${PROTOBUF_LIB_DIR}/libprotobuf.a
)

# 导出decode模块的目标
#set_target_properties(decoder_lib PROPERTIES EXPORT_NAME decode)