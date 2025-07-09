# mylogger/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# 自动收集源文件
file(GLOB_RECURSE MYLOGGER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
)

# 创建静态库
add_library(mylogger STATIC ${MYLOGGER_SOURCES}
        mmap/mmap_linux.cpp
        mmap/mmap_win.cpp
        ../defer.h)

# 设置包含目录（关键修改点）
target_include_directories(mylogger PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}  # 将mylogger目录设为根包含路径
)

# 平台特定源文件
if(UNIX AND NOT APPLE)
    target_sources(mylogger PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/utils/sys_util_linux.cpp
    )
elseif(WIN32)
    target_sources(mylogger PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/utils/sys_util_win.cpp
    )
endif()

# 其他设置保持不变...