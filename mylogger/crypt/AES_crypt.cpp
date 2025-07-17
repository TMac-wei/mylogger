/**
 * @file      AES_crypt.cpp
 * @brief     [简要说明此头文件的作用]
 * @author    Weijh
 * @version   1.0
 */

#include "crypt/AES_crypt.h"

#include "cryptopp/aes.h"
#include "cryptopp/base64.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/filters.h"
#include "cryptopp/hex.h"
#include "cryptopp/modes.h"
#include "cryptopp/oids.h"
#include "cryptopp/osrng.h"

namespace logger {
    namespace crypt {

        /// detail作用域内详细实现了随机密钥、随机向量、加密解密的功能
        namespace detail {
            using CryptoPP::byte;

            /***
             * @brief 生成随机密钥
             * @return 随机密钥
             *      使用AutoSeededRandomPool生成密码学安全的随机密钥，默认长度为 16 字节（128 位），并转换为十六进制字符串
             *      利用static限定作用域为本文件
             */
            static std::string GenerateKey() {
                CryptoPP::AutoSeededRandomPool rnd;
                byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
                rnd.GenerateBlock(key, sizeof(key));
                return BinaryKeyToHex(std::string(reinterpret_cast<const char *>(key), sizeof(key)));
            }

            /**
             * @brief 生成随机IV
             * @return
             */
            static std::string GenerateIV() {
                CryptoPP::AutoSeededRandomPool rnd;
                byte iv[CryptoPP::AES::BLOCKSIZE];
                rnd.GenerateBlock(iv, sizeof(iv));
                return BinaryKeyToHex(std::string(reinterpret_cast<const char *>(iv), sizeof(iv)));
            }


            /***
             * @brief 加密函数（使用CBC模式）
             * @param input
             * @param input_size
             * @param output
             * 加密流程：
             *      1、使用AES算法初始化加密器，传入密钥
             *      2、使用CBC（密码块链接）模式增强安全性，需要传入IV,且每次加密应使用不同的 IV
             *      3、通过StreamTransformationFilter将输入数据加密后输出到output字符串
             */
            void Encrypt(const void *input, size_t input_size, std::string &output,
                         const std::string &key, const std::string &iv) {
                CryptoPP::AES::Encryption aes_encryption(
                        reinterpret_cast<const byte *>(key.data()), key.size());
                CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(
                        aes_encryption, reinterpret_cast<const byte *>(iv.data()));
                CryptoPP::StreamTransformationFilter stf_encryptor(
                        cbc_encryption, new CryptoPP::StringSink(output),
                        CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING);  /// 显示指定填充方式
                stf_encryptor.Put(reinterpret_cast<const byte *>(input), input_size);
                stf_encryptor.MessageEnd();
            }

            /**
             * @brief 对称地初始化解密器，使用相同的密钥和 IV，将密文转换为明文
             * @param data
             * @param size
             * @return
             */
            static std::string Decrypt(const void *data, size_t size,
                                       const std::string &key, const std::string &iv) {
                std::string decryptedtext;
                CryptoPP::AES::Decryption aes_decryption(
                        reinterpret_cast<const byte *>(key.data()),
                        key.size());
                CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(
                        aes_decryption,
                        reinterpret_cast<const byte *>(iv.data()));

                CryptoPP::StreamTransformationFilter stf_decryptor(
                        cbc_decryption,
                        new CryptoPP::StringSink(decryptedtext));

                stf_decryptor.Put(reinterpret_cast<const byte *>(data), size);
                stf_decryptor.MessageEnd();
                return decryptedtext;
            }
        }

        /**
         * @brief AESCrypt构造函数
         * @param key
         *      利用随机IV，并转换密钥为二进制格式
         */
        AESCrypt::AESCrypt(std::string key) {
            key_ = HexKeyToBinary(std::move(key));
            /// 生成随机IV(二进制，16字节）并存储
            iv_ = HexKeyToBinary(detail::GenerateIV());

        }

        /**
         * @brief 生成十六进制格式的密钥，提供外部接口
         * @return 十六进制的密钥字符串
         *          调用detail中的GenerateKey
         */
        std::string AESCrypt::GenerateKey() {
            return detail::GenerateKey();
        }

        /**
         * @brief 生成十六进制格式的IV，提供外部接口
         * @return 十六进制的IV字符串
         *          调用detail中的GenerateIV
         */
        std::string AESCrypt::GenerateIV() {
            return detail::GenerateIV();
        }

        /**
         * @brief AES加密算法
         * @param input
         * @param input_size
         * @param output  输出字符串，利用引用输出，避免拷贝
         *      自动生成新的随机IV，附加到密文头部
         */
        void AESCrypt::Encrypt(const void *input, size_t input_size, std::string &output) {
            /// 每次生成新的随机IV
            std::string binary_iv = HexKeyToBinary(detail::GenerateIV());

            /// 加密数据
            std::string ciphertext;
            detail::Encrypt(input, input_size, ciphertext, key_, binary_iv);

            /// 密文格式[IV(16字节)] + [加密数据]（IV随密文传递，无需单独存储）
            output.reserve(binary_iv.size() + ciphertext.size());
            output.append(binary_iv);       /// 附加IV
            output.append(ciphertext);      /// 附加加密数据
        }

        /**
         * @brief 解密：从密文头部提取IV，再解密数据
         * @param data
         * @param size
         * @return
         */
        std::string AESCrypt::Decrypt(const void *data, size_t size) {
            /// 检查密文长度：至少包含16字节IV
            if (size < CryptoPP::AES::BLOCKSIZE) {
                throw std::invalid_argument("Invalid ciphertext: too short to contain IV");
            }

            /// 从密文头部提取IV（ 前16字节）
            const char *data_ptr = reinterpret_cast<const char *>(data);
            std::string binary_iv(data_ptr, CryptoPP::AES::BLOCKSIZE);

            /// 提取实际加密数据（IV之后的数据）
            const void *cipher_data = data_ptr + CryptoPP::AES::BLOCKSIZE;
            size_t cipher_size = size - CryptoPP::AES::BLOCKSIZE;

            /// 解密数据
            return detail::Decrypt(cipher_data, cipher_size, key_, binary_iv);
        }
    }
}