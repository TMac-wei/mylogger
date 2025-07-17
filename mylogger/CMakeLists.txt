# mylogger/CMakeLists.txt

include_guard(GLOBAL)

# 自动收集源文件，但是要排除第三方已经编译好的文件
file(GLOB_RECURSE MYLOGGER_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

# 手动移除 GTest 源码
foreach(file ${MYLOGGER_COMMON_SOURCES})
    if(file MATCHES ".*/third_party/.*"
            OR file MATCHES ".*/sys_util_linux.cpp"
            OR file MATCHES ".*/sys_util_win.cpp"
            OR file MATCHES ".*/mmap_linux.cpp"
            OR file MATCHES ".*/mmap_win.cpp")
        list(REMOVE_ITEM MYLOGGER_COMMON_SOURCES ${file})
    endif()
endforeach()

# 定义Protobuf路径
set(PROTOBUF_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/protobuf-21.8)
set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_ROOT}/include)
set(PROTOBUF_LIB_DIR ${PROTOBUF_ROOT}/lib)
set(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_ROOT}/bin/protoc.exe)

# 不使用find_package，直接使用本地编译的Protobuf
message(STATUS "使用本地Protobuf: ${PROTOBUF_ROOT}")
message(STATUS "Protobuf编译器: ${PROTOBUF_PROTOC_EXECUTABLE}")
message(STATUS "Protobuf头文件目录: ${PROTOBUF_INCLUDE_DIR}")
message(STATUS "Protobuf库目录: ${PROTOBUF_LIB_DIR}")

# 添加 Protobuf 生成规则
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/effective_msg.proto)
set(PROTO_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# 生成 Protobuf 代码
add_custom_command(
        OUTPUT
        ${PROTO_OUTPUT_DIR}/effective_msg.pb.cc
        ${PROTO_OUTPUT_DIR}/effective_msg.pb.h
        COMMAND
        ${PROTOBUF_PROTOC_EXECUTABLE}  # 使用本地protoc
        ARGS
        --cpp_out=${PROTO_OUTPUT_DIR}
        -I${CMAKE_CURRENT_SOURCE_DIR}/proto
        ${PROTO_FILE}
        DEPENDS
        ${PROTO_FILE}
        COMMENT "运行C++协议缓冲区编译器生成effective_msg.proto代码"
        VERBATIM
)

# 确保生成的文件被识别为源文件
set_source_files_properties(
        ${PROTO_OUTPUT_DIR}/effective_msg.pb.cc
        ${PROTO_OUTPUT_DIR}/effective_msg.pb.h
        PROPERTIES GENERATED TRUE
)


# 添加库目标
add_library(mylogger_lib STATIC ${MYLOGGER_COMMON_SOURCES}
        ${PROTO_OUTPUT_DIR}/effective_msg.pb.cc)



# 平台特定源文件
if(UNIX AND NOT APPLE)
    target_sources(mylogger_lib PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/utils/sys_util_linux.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/mmap/mmap_linux.cpp
    )
elseif(WIN32)
    target_sources(mylogger_lib PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/utils/sys_util_win.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/mmap/mmap_win.cpp
    )
endif()

# 创建静态库 头文件路径设置
target_include_directories(mylogger_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}     # for "space.h"
        ${CMAKE_SOURCE_DIR}             # for "defer.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cryptopp_cmake
        ${PROTOBUF_INCLUDE_DIR}         # Protobuf头文件
        ${PROTO_OUTPUT_DIR}             # 生成的Protobuf头文件
)

# 链接 Protobuf 库
target_link_libraries(mylogger_lib PRIVATE
        ${PROTOBUF_LIB_DIR}/libprotobuf.a  # 链接Protobuf静态库
        ${PROTOBUF_LIB_DIR}/libprotoc.a
)
