cmake_minimum_required(VERSION 3.10)
project(GoogleTest)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 Crypto++ 路径
set(CRYPTOPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mylogger/third_party/cryptopp_cmake)
set(CRYPTOPP_INCLUDE_DIR ${CRYPTOPP_DIR}/cryptopp)
set(CRYPTOPP_LIBRARY ${CRYPTOPP_DIR}/build/cryptopp/libcryptopp.a)

# 设置 Zstd 路径
set(ZSTD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mylogger/third_party/zstd)
set(ZSTD_INCLUDE_DIR ${ZSTD_DIR}/lib)
set(ZSTD_LIBRARY ${ZSTD_DIR}/build/cmake/build_mingw/lib/libzstd.a)

# 设置Protobuf路径
set(PROTOBUF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../mylogger/third_party/protobuf-21.8)
set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_ROOT}/include)
set(PROTOBUF_LIB_DIR ${PROTOBUF_ROOT}/lib)
set(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_ROOT}/bin/protoc.exe)

# 添加测试可执行文件
add_executable(google_test
        defer_test.cpp
        space_test.cpp
        thread_pool.cpp
        strand_executor.cpp
        context.cpp
        log_handle_test.cpp
        zlib_compress_test.cpp
        zstd_compress_test.cpp
        crypt_test.cpp
        proto_test.cpp
        effective_sink_test.cpp
)

# 设置头文件搜索路径
target_include_directories(google_test PRIVATE
        ${PROJECT_SOURCE_DIR}/..       # 根目录
        ${PROJECT_SOURCE_DIR}/../mylogger
        ${PROJECT_SOURCE_DIR}/../mylogger/third_party/googletest/googletest/include
        ${ZSTD_INCLUDE_DIR}           # Zstd 头文件
        ${CRYPTOPP_INCLUDE_DIR}
        ${PROTOBUF_INCLUDE_DIR}       # Protobuf 头文件
        ${CMAKE_CURRENT_BINARY_DIR}   # 生成的 Protobuf 头文件(使用当前二进制目录)
)

# 定义一个函数用于检查库是否存在
function(check_library LIB_PATH LIB_NAME)
    if(EXISTS ${LIB_PATH})
        message(STATUS "${LIB_NAME} 库已找到: ${LIB_PATH}")
    else()
        message(FATAL_ERROR "${LIB_NAME} 库未找到: ${LIB_PATH}")
    endif()
endfunction()

# 检查依赖库
check_library(${ZSTD_LIBRARY} "Zstd")
check_library(${CRYPTOPP_LIBRARY} "Crypto++")
check_library(${PROTOBUF_LIB_DIR}/libprotobuf.a "Protobuf")

# 链接所有依赖库
target_link_libraries(google_test PRIVATE
        mylogger_lib
        gtest
        gtest_main
        zlib
        ${ZSTD_LIBRARY}  # Zstd 静态库
        ${CRYPTOPP_LIBRARY}
        ${PROTOBUF_LIB_DIR}/libprotobuf.a  # Protobuf 静态库
)

enable_testing()
add_test(NAME google_test COMMAND google_test)

# 显示链接状态
message(STATUS "链接的库:")
message(STATUS "  mylogger_lib: $<TARGET_FILE:mylogger_lib>")
message(STATUS "  gtest: $<TARGET_FILE:gtest>")
message(STATUS "  gtest_main: $<TARGET_FILE:gtest_main>")
message(STATUS "  zlib: $<TARGET_FILE:zlib>")
message(STATUS "  zstd: ${ZSTD_LIBRARY}")
message(STATUS "  cryptopp: ${CRYPTOPP_LIBRARY}")
message(STATUS "  protobuf: ${PROTOBUF_LIB_DIR}/libprotobuf.a")