/**
 * @file      file_util.cpp
 * @brief     [获取文件大小]
 * @author    Weijh
 * @version   1.0
 */

#include "utils/file_util.h"

namespace logger {
    namespace fs {
        size_t GetFileSize(const std::filesystem::path &file_path) {
            /// 需要做存在性检测，如果不存在文件路径，返回0
            if (std::filesystem::exists(file_path)) {
                return std::filesystem::file_size(file_path);
            }
            return 0;
        }
    }
}