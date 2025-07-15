# 指定 MinGW64 编译器
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")

# 指定目标系统为 Windows
set(CMAKE_SYSTEM_NAME Windows)

# 可选：指定库文件和头文件路径（若 MinGW64 安装在非默认路径）
# set(CMAKE_FIND_ROOT_PATH "D:/msys64/mingw64")  # MinGW64 根目录
# set(CMAKE_INCLUDE_PATH "${CMAKE_FIND_ROOT_PATH}/include")
# set(CMAKE_LIBRARY_PATH "${CMAKE_FIND_ROOT_PATH}/lib")

# 优先从 MinGW64 路径查找库和头文件
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)