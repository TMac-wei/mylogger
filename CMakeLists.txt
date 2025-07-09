cmake_minimum_required(VERSION 3.30)
project(mylogger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动收集所有源文件
file(GLOB_RECURSE MYLOGGER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/**/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/**/*.cpp
)

file(GLOB_RECURSE MYLOGGER_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/**/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/mylogger/**/*.hpp
)

add_executable(mylogger main.cpp
        mylogger/mmap/mmap_aux.h
        mylogger/mmap/mmap_aux.cc
        mylogger/utils/file_util.cpp
        mylogger/utils/file_util.h
        mylogger/utils/sys_util.cpp
        mylogger/utils/sys_util.h
        mylogger/utils/time_count.h
        mylogger/utils/sys_util_linux.cpp
        mylogger/utils/sys_util_win.cpp
)
