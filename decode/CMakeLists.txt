#cmake_minimum_required(VERSION 3.20)
#project(decoder)
#
## 将decode_formatter转换为库
#add_library(decoder_lib STATIC
#        decode_formatter.cpp
#        decode.cpp
#)
#
## 配置 decode 库的头文件路径
#target_include_directories(decoder_lib
#        PUBLIC
#        ${CMAKE_CURRENT_SOURCE_DIR}  # decode 目录（供外部包含 decode_formatter.h）
#        ${CMAKE_CURRENT_SOURCE_DIR}/..  # 根目录（供包含 mylogger/...）
#        ${PROTOBUF_INCLUDE_DIR}  # Protobuf 头文件
#)
#
## 链接必要的库
#target_link_libraries(decoder_lib
#        PUBLIC
#        mylogger_lib
#        ${PROTOBUF_LIB_DIR}/libprotobuf.a
#)
#
## （可选）如果需要可执行程序
#add_executable(decoder_main decode.cpp)
#target_link_libraries(decoder_main PRIVATE decoder_lib)

cmake_minimum_required(VERSION 3.20)
project(decoder)

# 构建 decode 库
add_library(decoder_lib STATIC
        decode_formatter.cpp
        decode.cpp
)

# 头文件路径（仅需 decode 目录和根目录）
target_include_directories(decoder_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}  # decode 目录
        ${CMAKE_CURRENT_SOURCE_DIR}/..  # 根目录（mylogger）
)

# 链接依赖（直接依赖 mylogger_lib）
target_link_libraries(decoder_lib
        PUBLIC
        mylogger_lib  # mylogger_lib 已包含 Protobuf 依赖
)

# 可选：可执行程序
add_executable(decoder_main decode.cpp)
target_link_libraries(decoder_main PRIVATE decoder_lib)