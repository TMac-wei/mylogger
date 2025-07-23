/**
 * @file      effective_sink.cpp
 * @brief     [日志接收器实现，负责将日志消息压缩、加密后写入文件，并管理日志文件大小和声明周期]
 * @author    Weijh
 * @version   1.0
 */

#include "sinks/effective_sink.h"
#include "compress/zstd_compress.h"
#include "crypt/AES_crypt.h"
#include "defer.h"
#include "formatter/effective_formatter.h"
#include "internal_log.h"
#include "utils/file_util.h"
#include "utils/sys_util.h"
#include "utils/time_count.h"

#include <fstream>

namespace logger {

    /**
     * @brief EffectiveSink构造函数，初始化配置参数
     * @param conf
     *  初始化流程：
     *      1、创建日志目录（如果不存在）
     *      2、初始化任务执行器（异步处理文件操作）
     *      3、生成ECDH密钥对，与服务器建立共享密钥
     *      4、初始化AES加密器和Zstd压缩器
     *      5、创建两个内存映射文件（master_cache_和slave_cache_)
     *      6、检查缓存文件是否存在残留数据，如果有则触发写入文件
     *      7、设置定时器任务清理过期文件
     */
    EffectiveSink::EffectiveSink(logger::EffectiveSink::Conf conf) : conf_(std::move(conf)) {
        LOG_INFO(
                "EffectiveSink: file_dir = {}, file_prefix = {}, pub_key = {}, interval = {}, single_file_size = {}, total_files_sizes = {}",
                conf_.file_dir_.string(), conf_.file_prefix_, conf_.pub_key_, conf_.interval.count(),
                conf_.single_file_size_.count(), conf_.total_files_sizes_.count());

        if (!std::filesystem::exists(conf_.file_dir_)) {
            std::filesystem::create_directories(conf_.file_dir_);
        }

        task_runner_ = NEW_TASK_RUNNER(10086);
        formatter_ = std::make_unique<EffectiveFormatter>();
        auto ecdh_key = crypt::GenerateECDHKey();
        auto client_private_key = std::get<0>(ecdh_key);
        client_pub_key_ = std::get<1>(ecdh_key);

        LOG_INFO("EffectiveSink: client public size {}", client_pub_key_.size());
        /// 创建服务器公钥
        std::string server_pub_key_bin = crypt::HexKeyToBinary(conf_.pub_key_);
        /// 利用客户端私钥和服务端公钥生成共享密钥
        std::string shared_secret = crypt::GenerateECDHSharedKey(client_private_key, server_pub_key_bin);

        /// 初始化压缩器和加密器
        compress_ = std::make_unique<compress::ZstdCompress>();
        crypt_ = std::make_unique<crypt::AESCrypt>(shared_secret);

        master_cache_ = std::make_unique<MMapAux>(conf_.file_dir_ / "master_cache");
        slave_cache_ = std::make_unique<MMapAux>(conf_.file_dir_ / "slave_cache");
        /// 检查文件映射是否成功
        if (!master_cache_->IsValid_()|| !slave_cache_->IsValid_()) {
            throw std::runtime_error("EffectiveSink::EffectiveSink: create mmap failed");
        }

        /// 判断slave_cache_是否为空
        if (!slave_cache_->Empty()) {
            is_slave_free_.store(true);
            PrepareFile_();
            WAIT_TASK_IDLE(task_runner_);
        }

        /// 如果master_cache_满了，进行切换
        if (!master_cache_->Empty()) {
            if (is_slave_free_.load()) {
                is_slave_free_.store(false);
                SwitchCache_();
            }
            PrepareFile_();
        }

        /// 设置周期任务
        POST_REPEATED_TASK(task_runner_, [this]() {
            ElimateFiles_();
        }, conf_.interval, -1);

    }

    /**
     * 析构时等待所有任务完成
     */
    EffectiveSink::~EffectiveSink() {
        WAIT_TASK_IDLE(task_runner_);
    }


    /**
     * @brief  日志处理
     * @param msg
     *      日志处理流程
     *      1、格式化，使用EffectiveFormatter将日志消息转换为字节流
     *      2、使用Zstd压缩格式化后的日志，写入compress_buf_
     *      3、使用AES加密压缩后的数据，写入crypt_buf_
     *      4、将加密后的数据写入master_cache_，并且添加数据头部（包括大小信息）
     *      5、当master_cache_使用超过80%，切换到slave_cache并将master_cache写入文件
     */
    void EffectiveSink::Log(const logger::LogMsg &msg) {
        /// 每个线程都有独立的缓冲区，避免多线程竞争同一缓冲区
        static thread_local MemoryBuf buf;
        /// 将日志消息格式化到缓冲区
        formatter_->Format(msg, &buf);

        /// 压缩
        if (master_cache_->Empty()) {
            compress_->ResetStream();
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            /// reserve()仅预分配内存，在这个内存区间操作可能会造成未定义行为，这里改用resize
            compressed_buf_.resize(compress_->CompressedBound(buf.size()));
            size_t compressed_size = compress_->Compress(buf.data(), buf.size(),
                                                         compressed_buf_.data(), compressed_buf_.capacity());
            if (compressed_size == 0) {
                LOG_ERROR("EffectiveSink:log: compress failed!");
                return;
            }
            /// 调整为实际压缩后的大小
            compressed_buf_.resize(compressed_size);

            encrypt_buf_.clear();
            encrypt_buf_.reserve(compressed_size + 16);
            crypt_->Encrypt(compressed_buf_.data(), compressed_buf_.size(), encrypt_buf_);
            if (encrypt_buf_.empty()) {
                LOG_ERROR("EffectiveSink::Log: encrypt failed!");
                return;
            }
            WriteToCache_(encrypt_buf_.data(), encrypt_buf_.size());
        }
        /// 如果需要切换缓冲区了同时slave_cache_为空，则可以进行切换
        if (NeedCacheToFile_()) {
            if (is_slave_free_.load()) {
                is_slave_free_.store(false);
                SwitchCache_();
            }
            /// 却换缓冲区后，同样需要做文件准备等初始化操作
            PrepareFile_();
        }
    }

    void EffectiveSink::SetFormatter(std::unique_ptr<Formatter> formatter) {}

    /**
     * @brief 强制刷新接口，用于确保所有缓存的日志数据同步到磁盘
     */
    void EffectiveSink::Flush() {
        TIME_COUNT("Flush ");
        /// 提交异步写任务，将slave_cache_数据写入文件
        PrepareFile_();
        /// 阻塞当前线程，直到所有待处理的任务完成
        WAIT_TASK_IDLE(task_runner_);

        /// 检查是否有新数据需要写入,这里的目的是检测第一次刷新后，master中是否新添加了数据
        if (is_slave_free_.load()) {
            is_slave_free_.store(false);
            SwitchCache_();
        }

        /// 将master_cache_中残留的数据全都写入磁盘
        PrepareFile_();
        WAIT_TASK_IDLE(task_runner_);
    }

    /**
     * @brief 将日志数据从缓存中异步写入文件
     *      将CacheToFile_接口封装成任务，提交到task_runner_线程池中执行
     * 这样异步调度的目的是分离职责：
     *       日志写入线程专注接收新日志，快速返回
     *       文件IO线程负责耗时的磁盘写入操作，避免阻塞主线程
     */
    void EffectiveSink::PrepareFile_() {
        POST_TASK(task_runner_, [this]() {
            CacheToFile_();
        });
    }

    /**
     * @brief  将缓存数据写入磁盘文件的核心函数，采用异步方式执行避免阻塞日志收集
     * 双缓冲机制：master_cache_持续接收新日志，slave_cache_专注写入文件，通过缓存交换实现读写分离，降低锁竞争
     */
    void EffectiveSink::CacheToFile_() {
        TIME_COUNT("CacheToFile_"); /// 计时
        if (is_slave_free_.load()) {
            return;
        }

        if (slave_cache_->Empty()) {
            is_slave_free_.store(true);
            return;
        }

        {
            /// 将slave_cache_文件内容写入文件
            auto file_path = GetFilePath_();
            detail::LogChunkHeader Log_chunk_header;
            Log_chunk_header.size = slave_cache_->Size();
            /// 将用户公钥拷贝到log_chunk_header中
            memcpy(Log_chunk_header.pub_key, client_pub_key_.data(), client_pub_key_.size());

            /// 以追加模式打开文件并且写入数据
            std::ofstream ofs(file_path, std::ios::binary | std::ios::app);
            /// 检查文件打开和写入的状态，记录错误日志
            if (!ofs.is_open()) {
                LOG_ERROR("EffectiveSink::CacheToFile_ failed to open file: {}", file_path.string());
                return;
            }

            ofs.write(reinterpret_cast<char *>(&Log_chunk_header), sizeof(Log_chunk_header));
            ofs.write(reinterpret_cast<char *>(slave_cache_->Data()), Log_chunk_header.size);
            if (!ofs) {
                LOG_ERROR("EffectiveSink::CacheToFile_ failed to write to file: {}", file_path.string());
            }
            ofs.close();
        }
        slave_cache_->Clear();      /// 清空slave_cache缓存
        is_slave_free_.store(true);     /// 标记为可用

    }

    /**
     * @brief 交换master_cache_和slave_cache_
     */
    void EffectiveSink::SwitchCache_() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(master_cache_, slave_cache_);
    }

    /**
     * @brief    判断是否需要从缓存写入到文件中
     * @return  判断的结果
     * 如果占比 > 0.8 表示可以切换
     */
    bool EffectiveSink::NeedCacheToFile_() {
        return master_cache_->GetRatio() > 0.8;
    }

    /**
     * @brief  将日志信息写入文件
     * @param data 日志信息内容
     * @param size 日志信息大小
     */
    void EffectiveSink::WriteToCache_(const void *data, uint32_t size) {
        detail::LogItemHeader log_item_header;
        /// 设置实际数据的大小
        log_item_header.size = size;
        master_cache_->Push(&log_item_header, sizeof(log_item_header));
        master_cache_->Push(data, size);
    }

    /**
     * @brief  生成和管理日志文件路径
     * @return 文件路径
     *      1、根据配置规则动态生成符合命名规范的日志文件路径，实现日志文件的滚动机制
     *      2、根据单个文件大小限制触发日志文件滚动，避免单个文件过大
     *      3、处理同一个时间戳下的文件冲突，通过索引递增的方式确保文件名唯一
     */
    std::filesystem::path EffectiveSink::GetFilePath_() {
        /// 内部辅助函数，生成基于当前时间的路径前缀
        auto GetDateTimePathPrefix = [this]() -> std::filesystem::path {
            /// 获取当前时间戳
            std::time_t now = std::time(nullptr);
            std::tm tm;
            /// 转换为本地时间
            LocalTime(&tm, &now);
            char time_buf[32] = {0};
            /// 将当前时间进行格式化
            std::strftime(time_buf, sizeof(time_buf), "%Y%m%d%H%M%S", &tm);
            /// 路径组合：通过当前目录+前缀——时间戳格式
            return (conf_.file_dir_ / (conf_.file_prefix_ + "_" + time_buf));
        };

        if (log_file_path_.empty()) {
            /// 没有这个文件路径，直接生成
            log_file_path_ = GetDateTimePathPrefix().string() + ".log";
        } else {
            /// 已经有了，需要文件滚动；首先判断文件大小是否超过限制
            auto file_size = fs::GetFileSize(log_file_path_);
            bytes single_file_bytes = space_cast<bytes>(conf_.single_file_size_);

            /// 如果当前文件大小超过限制了就需要回滚生成新的日志文件
            if (file_size > single_file_bytes.count()) {
                auto date_time_path = GetDateTimePathPrefix();
                auto file_path = date_time_path.string() + ".log";

                /// 检查刚才生成的路径是否存在了，因为同一个时间戳下可能生成多个文件
                if (std::filesystem::exists(file_path)) {
                    /// 统计同一个时间戳下的文件数量，生成递增索引
                    int index = 0;
                    for (auto &p: std::filesystem::directory_iterator(conf_.file_dir_)) {
                        /// 查找包含当前时间戳的文件
                        if (p.path().filename().string().find(date_time_path.string()) != std::string::npos) {
                            ++index;
                        }
                    }

                    /// 生成新的带索引的路径
                    log_file_path_ = date_time_path.string() + "_" + std::to_string(index) + ".log";
                } else {
                    /// 如果检索的路径不存在，直接使用这个路径
                    log_file_path_ = file_path;
                }
            }
        }
        LOG_INFO("EffectiveSink::GetFilePath_: log_file_path={}", log_file_path_.string());
        return log_file_path_;
    }


    /**
     *  负责管理日志文件的生命周期，定时清理旧日志文件，确保日志总存储量不超过配置上限
     *  实现过程：
     *      1、扫描日志目录下的所有.log后缀文件
     *      2、按文件最后修改时间排序
     *      3、累加文件大小，当总大小超过配置的total_file_size_时，进行淘汰（淘汰策略参考LRU)
     */
    void EffectiveSink::ElimateFiles_() {
        LOG_INFO("EffectiveSink::ElimateFiles_ : start");
        /// 收集所有.log文件
        std::vector<std::filesystem::path> log_files;
        for (auto &file: std::filesystem::directory_iterator(conf_.file_dir_)) {
            if (file.path().extension() == ".log") {
                log_files.push_back(file.path());
            }
        }

        /// 将收集到的文件按照最近修改时间排序，最近修改在前
        std::sort(log_files.begin(), log_files.end(),
                  [](const std::filesystem::path &lhs, const std::filesystem::path &rhs) {
                      return std::filesystem::last_write_time(lhs) > std::filesystem::last_write_time(rhs);
                  });

        /// 现在计算所有的文件总和，首先将其转换为字节大小
        size_t total_bytes = space_cast<bytes>(conf_.total_files_sizes_).count();
        size_t used_bytes = 0;

        for (auto &file: log_files) {
            /// 统计文件总大小
            used_bytes += fs::GetFileSize(file);
            if (used_bytes > total_bytes) {
                /// 移除最久未修改的那个文件
                LOG_INFO("EffectiveSink::ElimateFiles_: remove file = {}", file.string());
//                std::filesystem::remove(file);
                if (!std::filesystem::remove(file)) { // 检查删除结果
                    LOG_WARN("EffectiveSink::ElimateFiles_: Failed to remove file: {}", file.string());
                }
            }
        }
    }

}