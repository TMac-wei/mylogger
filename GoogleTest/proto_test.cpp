/**
 * @file      proto_test.cpp
 * @brief     Protobuf消息测试用例，包含版本验证、字段操作、序列化等测试
 * @author    Weijh
 * @version   1.0
 */

#include <gtest/gtest.h>
#include "proto/effective_msg.pb.h"
#include <string>
#include <cstdint>
#include <iostream>  // 用于打印版本信息

// 测试消息的基本字段赋值与获取
TEST(EffectiveMsgTest, FieldAccess) {
    EffectiveMsg msg;

    // 赋值测试
    msg.set_level(3);               // WARN 级别
    msg.set_timestamp(1620000000);  // 示例时间戳
    msg.set_pid(1234);
    msg.set_tid(5678);
    msg.set_line(42);
    msg.set_file_name("logger.cpp");
    msg.set_func_name("LogWarning");
    msg.set_log_info("Disk space low");

    // 获取值并验证
    EXPECT_EQ(msg.level(), 3);
    EXPECT_EQ(msg.timestamp(), 1620000000);
    EXPECT_EQ(msg.pid(), 1234);
    EXPECT_EQ(msg.tid(), 5678);
    EXPECT_EQ(msg.line(), 42);
    EXPECT_EQ(msg.file_name(), "logger.cpp");
    EXPECT_EQ(msg.func_name(), "LogWarning");
    EXPECT_EQ(msg.log_info(), "Disk space low");
}

// 测试消息的默认值
TEST(EffectiveMsgTest, DefaultValues) {
    const EffectiveMsg& default_msg = EffectiveMsg::default_instance();

    // 数值类型默认值为0，字符串默认值为空
    EXPECT_EQ(default_msg.level(), 0);
    EXPECT_EQ(default_msg.timestamp(), 0);
    EXPECT_EQ(default_msg.pid(), 0);
    EXPECT_EQ(default_msg.tid(), 0);
    EXPECT_EQ(default_msg.line(), 0);
    EXPECT_TRUE(default_msg.file_name().empty());
    EXPECT_TRUE(default_msg.func_name().empty());
    EXPECT_TRUE(default_msg.log_info().empty());
}

// 测试消息的序列化与反序列化
TEST(EffectiveMsgTest, Serialization) {
    // 构建原始消息
    EffectiveMsg original;
    original.set_level(4);               // ERROR 级别
    original.set_timestamp(1630000000);
    original.set_pid(9876);
    original.set_tid(5432);
    original.set_line(100);
    original.set_file_name("error.cpp");
    original.set_func_name("HandleError");
    original.set_log_info("Connection timeout");

    // 序列化到二进制
    std::string serialized;
    ASSERT_TRUE(original.SerializeToString(&serialized));
    EXPECT_FALSE(serialized.empty());
    std::cout << "序列化成功，二进制数据长度: " << serialized.size() << " 字节" << std::endl;

    // 从二进制反序列化
    EffectiveMsg deserialized;
    ASSERT_TRUE(deserialized.ParseFromString(serialized));

    // 验证反序列化结果与原始消息一致
    EXPECT_EQ(deserialized.level(), original.level());
    EXPECT_EQ(deserialized.timestamp(), original.timestamp());
    EXPECT_EQ(deserialized.pid(), original.pid());
    EXPECT_EQ(deserialized.tid(), original.tid());
    EXPECT_EQ(deserialized.line(), original.line());
    EXPECT_EQ(deserialized.file_name(), original.file_name());
    EXPECT_EQ(deserialized.func_name(), original.func_name());
    EXPECT_EQ(deserialized.log_info(), original.log_info());
}

// 测试消息的清空操作
TEST(EffectiveMsgTest, Clear) {
    EffectiveMsg msg;
    msg.set_level(2);               // INFO 级别
    msg.set_log_info("System started");

    // 清空消息
    msg.Clear();

    // 验证所有字段恢复默认值
    EXPECT_EQ(msg.level(), 0);
    EXPECT_TRUE(msg.log_info().empty());
    EXPECT_EQ(msg.timestamp(), 0);  // 其他未赋值字段也应为默认值
}

// 测试消息的复制与移动
TEST(EffectiveMsgTest, CopyAndMove) {
    // 构建源消息
    EffectiveMsg source;
    source.set_level(1);               // DEBUG 级别
    source.set_file_name("debug.cpp");

    // 测试复制构造
    EffectiveMsg copy_constructed(source);
    EXPECT_EQ(copy_constructed.level(), source.level());
    EXPECT_EQ(copy_constructed.file_name(), source.file_name());

    // 测试赋值运算符
    EffectiveMsg copy_assigned;
    copy_assigned = source;
    EXPECT_EQ(copy_assigned.level(), source.level());
    EXPECT_EQ(copy_assigned.file_name(), source.file_name());

    // 测试移动构造
    EffectiveMsg moved(std::move(source));
    EXPECT_EQ(moved.level(), 1);
    EXPECT_EQ(moved.file_name(), "debug.cpp");

    // 移动后源消息应处于有效但未定义的状态（通常为默认值）
    EXPECT_EQ(source.level(), 0);  // 移动后可能被重置
}

//int main(int argc, char **argv) {
//    // 初始化Google Test并打印Protobuf版本
//    testing::InitGoogleTest(&argc, argv);
//
//    return RUN_ALL_TESTS();
//}