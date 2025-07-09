/**
 * @file      file_util.h
 * @brief     [获取文件大小]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <stdint.h>
#include <filesystem>

namespace logger {
    namespace fs {
        /// 获取文件大小 参数是文件路径
        size_t GetFileSize(const std::filesystem::path &file_path);
    }
}




