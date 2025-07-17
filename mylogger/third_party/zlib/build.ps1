# build.ps1

# 进入zlib目录
Set-Location -Path $PSScriptRoot

# 创建build目录
if (-not (Test-Path -Path ".\build")) {
    New-Item -ItemType Directory -Name "build"
}

# 进入build目录
Set-Location -Path ".\build"

# 使用CMake生成构建文件（指定MinGW Makefiles）
cmake .. -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="$PSScriptRoot\install"

# 编译
cmake --build .

# 安装（复制文件到install目录）
cmake --install .