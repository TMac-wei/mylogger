/**
 * @file      crypt_test.cpp
 * @brief     [ECDH-AES加密解密测试]
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include <iostream>
#include "crypt/crypt.h"
#include "crypt/AES_crypt.h"

// 打印版本信息
class CryptTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        std::cout << "===== Crypto Library Test Suite =====" << std::endl;
        std::cout << "Version: 1.0 (Optimized)" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
};

// 测试ECDH密钥交换功能
TEST_F(CryptTest, ECDHKeyExchangeTest) {
    std::cout << "[TEST] ECDH Key Exchange" << std::endl;

    try {
        // 生成客户端密钥对
        auto [client_pri, client_pub] = logger::crypt::GenerateECDHKey();
        std::cout << "Client private key size: " << client_pri.size() << " bytes" << std::endl;
        std::cout << "Client public key size: " << client_pub.size() << " bytes" << std::endl;

        // 生成服务器密钥对
        auto [server_pri, server_pub] = logger::crypt::GenerateECDHKey();
        std::cout << "Server private key size: " << server_pri.size() << " bytes" << std::endl;
        std::cout << "Server public key size: " << server_pub.size() << " bytes" << std::endl;

        // 客户端生成共享密钥（使用客户端私钥和服务器公钥）
        std::string client_shared =
                logger::crypt::GenerateECDHSharedKey(client_pri, server_pub);
        std::cout << "Client shared key size: " << client_shared.size() << " bytes" << std::endl;

        // 服务器生成共享密钥（使用服务器私钥和客户端公钥）
        std::string server_shared =
                logger::crypt::GenerateECDHSharedKey(server_pri, client_pub);
        std::cout << "Server shared key size: " << server_shared.size() << " bytes" << std::endl;

        // 验证共享密钥是否相同
        EXPECT_EQ(client_shared, server_shared);
        std::cout << "ECDH Key Exchange SUCCESS" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ECDH Key Exchange FAIL: " << e.what() << std::endl;
        FAIL();
    }
}

// 测试AES加密/解密功能
TEST_F(CryptTest, AESEncryptionDecryptionTest) {
    std::cout << "[TEST] AES Encryption/Decryption" << std::endl;

    try {
        // 生成AES密钥
        std::string aes_key = logger::crypt::AESCrypt::GenerateKey();
        std::cout << "Generated AES Key (hex): " << aes_key << std::endl;

        // 创建加密器
        logger::crypt::AESCrypt crypt(aes_key);

        // 测试数据
        std::string plaintext = "Hello, this is a test message for encryption!";
        std::cout << "Plaintext: " << plaintext << std::endl;

        // 加密
        std::string ciphertext;
        crypt.Encrypt(plaintext.data(), plaintext.size(), ciphertext);
        std::cout << "Ciphertext size: " << ciphertext.size() << " bytes" << std::endl;

        // 解密
        std::string decrypted = crypt.Decrypt(ciphertext.data(), ciphertext.size());
        std::cout << "Decrypted text: " << decrypted << std::endl;

        // 验证解密结果
        EXPECT_EQ(plaintext, decrypted);
        std::cout << "AES Encryption/Decryption SUCCESS" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "AES Encryption/Decryption FAIL: " << e.what() << std::endl;
        FAIL();
    }
}

// 测试ECDH和AES结合使用
TEST_F(CryptTest, CombinedECDHAESTest) {
    std::cout << "[TEST] Combined ECDH + AES" << std::endl;

    try {
        // 1. ECDH密钥交换
        auto [client_pri, client_pub] = logger::crypt::GenerateECDHKey();
        auto [server_pri, server_pub] = logger::crypt::GenerateECDHKey();

        std::string shared_key =
                logger::crypt::GenerateECDHSharedKey(client_pri, server_pub);

        // 将共享密钥转换为十六进制用于AES
        std::string hex_shared_key = logger::crypt::BinaryKeyToHex(shared_key);
        std::cout << "Shared Key (hex): " << hex_shared_key << std::endl;

        // 2. 使用共享密钥进行AES加密/解密
        logger::crypt::AESCrypt crypt(hex_shared_key);

        // 测试数据
        std::string test_data = "Sensitive data transmitted over insecure channel";
        std::cout << "Original Data: " << test_data << std::endl;

        // 加密
        std::string encrypted;
        crypt.Encrypt(test_data.data(), test_data.size(), encrypted);
        std::cout << "Encrypted Size: " << encrypted.size() << " bytes" << std::endl;

        // 解密
        std::string decrypted = crypt.Decrypt(encrypted.data(), encrypted.size());
        std::cout << "Decrypted Data: " << decrypted << std::endl;

        // 验证
        EXPECT_EQ(test_data, decrypted);
        std::cout << "Combined ECDH + AES SUCCESS" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Combined ECDH + AES FAIL: " << e.what() << std::endl;
        FAIL();
    }
}

//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
