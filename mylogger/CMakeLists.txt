# mylogger/CMakeLists.txt

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

# 添加库目标
add_library(mylogger_lib STATIC ${MYLOGGER_COMMON_SOURCES})



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
)