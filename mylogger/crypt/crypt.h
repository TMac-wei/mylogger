/**
 * @file      crypt.h
 * @brief     [ECDH非对称加密实现]
 * @author    Weijh
 * @version   1.0
 */

#pragma once

#include <memory>
#include <string>
#include <tuple>

namespace logger {
    namespace crypt {

        /// ECDH密钥交换接口
        /// 1、生成各自的公钥和私钥
        std::tuple<std::string, std::string> GenerateECDHKey();

        /// 2、生成共享加密公钥，利用椭圆曲线方法
        std::string GenerateECDHSharedKey(const std::string &client_pri, const std::string &server_pub);

        /// 密钥交换格式转换, 二进制 <===> 十六进制
        std::string BinaryKeyToHex(const std::string &binary_data);

        std::string HexKeyToBinary(const std::string &hex_data);

        /// 通用加密接口 ---> 抽象基类（支持动态实现）
        class Crypt {
        public:
            virtual ~Crypt() = default;

            /// 将输入数据加密到输出字符串
            virtual void Encrypt(const void *input, size_t input_size, std::string &output) = 0;

            /// 解密数据并返回原始内容
            virtual std::string Decrypt(const void *data, size_t size) = 0;

        };
    }
}