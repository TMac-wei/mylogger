/**
 * @file      crypt.cpp
 * @brief     [ECDH加密实现]
 * @author    Weijh
 * @version   1.0
 */

#include "crypt/crypt.h"

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

        using CryptoPP::byte;

        /**
         * @brief 将二进制数据转换为十进制数据
         * @param binary_data
         * @return 十进制字符串
         *      步骤：
         *      1、使用HexEncoder进行编码
         *      2、通过StringSink将结果输出到hex_data
         *      3、MessageEnd()确保编码完成
         *  潜在风险：string以\0作为结束符，若密钥包含\0会导致数据截断
         */
        std::string BinaryKeyToHex(const std::string &binary_data) {
            std::string hex_data;
            CryptoPP::HexEncoder encoder;
            encoder.Attach(new CryptoPP::StringSink(hex_data));
            encoder.Put(reinterpret_cast<const byte *>(binary_data.data()), binary_data.size());
            encoder.MessageEnd();
            return hex_data;
        }

        /**
         * @brief 将十进制数据转换为二进制数据
         * @param binary_data
         * @return 二进制字符串
         */
        std::string HexKeyToBinary(const std::string &hex_data) {
            std::string binary_data;
            CryptoPP::HexDecoder decoder;
            decoder.Attach(new CryptoPP::StringSink(binary_data));
            decoder.Put(reinterpret_cast<const byte *>(hex_data.data()), hex_data.size());
            decoder.MessageEnd();
            return binary_data;
        }

        /**
         * @brief 生成密钥对
         * @return 密钥对元组
         */
        std::tuple<std::string, std::string> GenerateECDHKey() {
            CryptoPP::AutoSeededRandomPool rnd;
            /// 使用secp256r1椭圆曲线（NIST P-256）
            CryptoPP::ECDH<CryptoPP::ECP>::Domain dh(CryptoPP::ASN1::secp256r1());
            /// SecByteBlock管理密钥内存，确保安全擦除
            CryptoPP::SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength());
            /// 生成密钥对
            dh.GenerateKeyPair(rnd, priv, pub);
            /// 返回string类型的密钥
            return std::make_tuple(std::string(reinterpret_cast<const char *>(priv.data()), priv.size()),
                                   std::string(reinterpret_cast<const char *>(pub.data()), pub.size()));
        }

        /**
         * @brief 共享密钥生成，通过客户端私钥和服务器公钥计算共享密钥
         * @param client_pri
         * @param server_pub
         * @return  共享密钥字符串
         */
        std::string GenerateECDHSharedKey(const std::string &client_pri, const std::string &server_pub) {
            using CryptoPP::byte;
            CryptoPP::AutoSeededRandomPool rnd;
            /// 使用相同的secp256r1曲线确保兼容性
            CryptoPP::ECDH<CryptoPP::ECP>::Domain dh(CryptoPP::ASN1::secp256r1());
            /// dh.Agree()验证密钥有效性并计算共享值
            CryptoPP::SecByteBlock shared(dh.AgreedValueLength());
            CryptoPP::SecByteBlock pub(reinterpret_cast<const byte *>(server_pub.data()), server_pub.size());
            CryptoPP::SecByteBlock priv(reinterpret_cast<const byte *>(client_pri.data()), client_pri.size());
            /// 有效性检查
            if (!dh.Agree(shared, priv, pub)) {
                throw std::runtime_error("Failed to reach shared secret");
            }
            return std::string(reinterpret_cast<const char *>(shared.data()), shared.size());
        }


    }
}
