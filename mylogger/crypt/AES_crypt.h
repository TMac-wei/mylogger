/**
 * @file      AES_crypt.h
 * @brief     [AES加密]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include "crypt/crypt.h"

namespace logger {
    namespace crypt {

        /// AES加密算法，继承自Crypt抽象基类
        class AESCrypt final : public Crypt {
        public:
            AESCrypt(std::string key);

            ~AESCrypt() override = default;

            /// 密钥和IV生成
            static std::string GenerateKey();

            static std::string GenerateIV();

            /// 核心：加密解密接口
            void Encrypt(const void *input, size_t input_size, std::string &output) override;

            std::string Decrypt(const void *data, size_t size) override;

        private:
            std::string key_;           /// 初始密钥
            std::string iv_;            /// 初始向量
        };
    }
}