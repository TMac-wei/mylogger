#include "space.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <type_traits>
#include <iostream>

// 辅助宏，用于输出带颜色和测试用例名称的日志
#define TEST_LOG(msg) \
    std::cout << "\033[36m[" << ::testing::UnitTest::GetInstance()->current_test_info()->name() \
              << "]\033[0m " << msg << std::endl;

// 测试基本构造与初始化
TEST(SpaceConstructionTest, BasicInitialization) {
TEST_LOG("开始测试基本构造函数...");

TEST_LOG("测试默认构造函数");
logger::bytes b_default;
EXPECT_EQ(b_default.count(), 0);
TEST_LOG("默认构造函数测试通过");

TEST_LOG("测试带值构造函数");
logger::kilobytes kb(2);
EXPECT_EQ(kb.count(), 2);
logger::megabytes mb(3);
EXPECT_EQ(mb.count(), 3);
TEST_LOG("带值构造函数测试通过");

TEST_LOG("测试拷贝构造函数");
logger::gigabytes gb1(4);
logger::gigabytes gb2(gb1);
EXPECT_EQ(gb2.count(), 4);
TEST_LOG("拷贝构造函数测试通过");

TEST_LOG("测试移动构造函数");
logger::terabytes tb1(5);
logger::terabytes tb2(std::move(tb1));
EXPECT_EQ(tb2.count(), 5);
TEST_LOG("移动构造函数测试通过");

TEST_LOG("基本构造函数测试完成\n");
}

// 测试单位转换(space_cast)
TEST(SpaceCastTest, UnitConversion) {
TEST_LOG("开始测试单位转换...");

TEST_LOG("测试 KB → B 转换");
logger::kilobytes kb1(1);
TEST_LOG("原始值: 1 KB = " << kb1.count() << " KB");
auto b1 = logger::space_cast<logger::bytes>(kb1);
TEST_LOG("转换后: " << b1.count() << " B");
EXPECT_EQ(b1.count(), 1024ULL);
TEST_LOG("KB → B 转换测试通过");

TEST_LOG("测试 MB → KB 转换");
logger::megabytes mb1(1);
TEST_LOG("原始值: 1 MB = " << mb1.count() << " MB");
auto kb2 = logger::space_cast<logger::kilobytes>(mb1);
TEST_LOG("转换后: " << kb2.count() << " KB");
EXPECT_EQ(kb2.count(), 1024LL);
TEST_LOG("MB → KB 转换测试通过");

TEST_LOG("测试 GB → MB 转换");
logger::gigabytes gb1(1);
TEST_LOG("原始值: 1 GB = " << gb1.count() << " GB");
auto mb2 = logger::space_cast<logger::megabytes>(gb1);
TEST_LOG("转换后: " << mb2.count() << " MB");
EXPECT_EQ(mb2.count(), 1024LL);
TEST_LOG("GB → MB 转换测试通过");

TEST_LOG("测试 TB → GB 转换");
logger::terabytes tb1(1);
TEST_LOG("原始值: 1 TB = " << tb1.count() << " TB");
auto gb2 = logger::space_cast<logger::gigabytes>(tb1);
TEST_LOG("转换后: " << gb2.count() << " GB");
EXPECT_EQ(gb2.count(), 1024LL);
TEST_LOG("TB → GB 转换测试通过");

TEST_LOG("测试浮点数转换");
logger::bytes b2(2048);
TEST_LOG("原始值: 2048 B = " << b2.count() << " B");
logger::space<double, logger::kilo> kb_float = logger::space_cast<logger::space<double, logger::kilo>>(b2);
TEST_LOG("转换后: " << kb_float.count() << " KB");
EXPECT_NEAR(kb_float.count(), 2.0, 1e-6);
TEST_LOG("浮点数转换测试通过");

TEST_LOG("测试跨单位转换");
logger::megabytes mb3(1);
TEST_LOG("原始值: 1 MB = " << mb3.count() << " MB");
auto b3 = logger::space_cast<logger::bytes>(mb3);
TEST_LOG("转换后: " << b3.count() << " B");
EXPECT_EQ(b3.count(), 1024ULL * 1024);
TEST_LOG("跨单位转换测试通过");

TEST_LOG("单位转换测试完成\n");
}

// 测试同单位算术运算
TEST(SpaceArithmeticTest, SameUnitOperations) {
TEST_LOG("开始测试同单位算术运算...");

TEST_LOG("测试加法运算");
logger::bytes b1(100), b2(200);
TEST_LOG("操作数: " << b1.count() << " B + " << b2.count() << " B");
auto b_add = b1 + b2;
TEST_LOG("结果: " << b_add.count() << " B");
EXPECT_EQ(b_add.count(), 300ULL);
static_assert(std::is_same_v<decltype(b_add), logger::bytes>, "同单位加法应保持原类型");
TEST_LOG("加法运算测试通过");

TEST_LOG("测试减法运算");
logger::kilobytes kb1(500), kb2(200);
TEST_LOG("操作数: " << kb1.count() << " KB - " << kb2.count() << " KB");
auto kb_sub = kb1 - kb2;
TEST_LOG("结果: " << kb_sub.count() << " KB");
EXPECT_EQ(kb_sub.count(), 300LL);
TEST_LOG("减法运算测试通过");

TEST_LOG("测试乘法运算(与算术类型)");
logger::megabytes mb1(2);
TEST_LOG("操作数: " << mb1.count() << " MB * 3");
auto mb_mul = mb1 * 3;
TEST_LOG("结果: " << mb_mul.count() << " MB");
EXPECT_EQ(mb_mul.count(), 6LL);
TEST_LOG("乘法运算测试通过");

TEST_LOG("测试除法运算(与算术类型)");
logger::gigabytes gb1(8);
TEST_LOG("操作数: " << gb1.count() << " GB / 2");
auto gb_div = gb1 / 2;
TEST_LOG("结果: " << gb_div.count() << " GB");
EXPECT_EQ(gb_div.count(), 4LL);
TEST_LOG("除法运算测试通过");

TEST_LOG("测试自增运算");
logger::bytes b_inc(10);
TEST_LOG("初始值: " << b_inc.count() << " B");
++b_inc;
TEST_LOG("前置自增后: " << b_inc.count() << " B");
EXPECT_EQ(b_inc.count(), 11ULL);
b_inc++;
TEST_LOG("后置自增后: " << b_inc.count() << " B");
EXPECT_EQ(b_inc.count(), 12ULL);
TEST_LOG("自增运算测试通过");

TEST_LOG("测试自减运算");
logger::kilobytes kb_dec(5);
TEST_LOG("初始值: " << kb_dec.count() << " KB");
--kb_dec;
TEST_LOG("前置自减后: " << kb_dec.count() << " KB");
EXPECT_EQ(kb_dec.count(), 4LL);
kb_dec--;
TEST_LOG("后置自减后: " << kb_dec.count() << " KB");
EXPECT_EQ(kb_dec.count(), 3LL);
TEST_LOG("自减运算测试通过");

TEST_LOG("同单位算术运算测试完成\n");
}

// 测试不同单位算术运算(自动统一精度)
TEST(SpaceArithmeticTest, CrossUnitOperations) {
TEST_LOG("开始测试不同单位算术运算...");

TEST_LOG("测试字节 + KB (高精度+低精度)");
logger::bytes b(100);
logger::kilobytes kb(1);
TEST_LOG("操作数: " << b.count() << " B + " << kb.count() << " KB");
auto cross_add = b + kb;
TEST_LOG("结果: " << cross_add.count() << " B");
EXPECT_EQ(cross_add.count(), 100 + 1024);
static_assert(std::is_same_v<decltype(cross_add)::period, std::ratio<1>>, "应统一为更高精度(字节)");
TEST_LOG("字节 + KB 测试通过");

TEST_LOG("测试 MB + GB (不同单位)");
logger::megabytes mb(512);
logger::gigabytes gb(1);
TEST_LOG("操作数: " << mb.count() << " MB + " << gb.count() << " GB");
auto cross_sub = gb - mb;
TEST_LOG("结果: " << cross_sub.count() << " MB");
EXPECT_EQ(cross_sub.count(), 1024 - 512);
static_assert(std::is_same_v<decltype(cross_sub)::period, logger::mega::type>, "应统一为MB");
TEST_LOG("MB + GB 测试通过");

TEST_LOG("测试除法运算(不同单位比值)");
logger::kilobytes kb2(2048);
logger::megabytes mb2(1);
TEST_LOG("操作数: " << kb2.count() << " KB / " << mb2.count() << " MB");
auto ratio = kb2 / mb2;
TEST_LOG("结果: " << ratio);
EXPECT_EQ(ratio, 2.0);
TEST_LOG("除法运算测试通过");

TEST_LOG("测试取模运算");
logger::bytes b_mod(2500);
logger::kilobytes kb_mod(2);
TEST_LOG("操作数: " << b_mod.count() << " B % " << kb_mod.count() << " KB");
auto mod_result = b_mod % kb_mod;
TEST_LOG("结果: " << mod_result.count() << " B");
EXPECT_EQ(mod_result.count(), 2500 - 2048);
TEST_LOG("取模运算测试通过");

TEST_LOG("不同单位算术运算测试完成\n");
}

// 测试与算术类型的混合运算
TEST(SpaceArithmeticTest, MixedWithArithmetic) {
TEST_LOG("开始测试与算术类型的混合运算...");

TEST_LOG("测试 space + 算术类型");
logger::bytes b1(100);
TEST_LOG("操作数: " << b1.count() << " B + 200");
auto b_add_num = b1 + 200;
TEST_LOG("结果: " << b_add_num.count() << " B");
EXPECT_EQ(b_add_num.count(), 300ULL);
TEST_LOG("space + 算术类型测试通过");

TEST_LOG("测试 算术类型 + space");
TEST_LOG("操作数: 300 + " << b1.count() << " B");
auto b_num_add = 300 + b1;
TEST_LOG("结果: " << b_num_add.count() << " B");
EXPECT_EQ(b_num_add.count(), 400ULL);
TEST_LOG("算术类型 + space测试通过");

TEST_LOG("测试 space * 算术类型(浮点数)");
logger::megabytes mb1(2);
TEST_LOG("操作数: " << mb1.count() << " MB * 3.5");
auto mb_mul_num = mb1 * 3.5;
TEST_LOG("结果: " << mb_mul_num.count() << " MB");
EXPECT_EQ(mb_mul_num.count(), 7.0);
TEST_LOG("space * 浮点数测试通过");

TEST_LOG("测试 算术类型 * space");
TEST_LOG("操作数: 4 * " << mb1.count() << " MB");
auto kb_num_mul = 4 * mb1;
TEST_LOG("结果: " << kb_num_mul.count() << " MB");
EXPECT_EQ(kb_num_mul.count(), 8LL);
TEST_LOG("算术类型 * space测试通过");

TEST_LOG("与算术类型的混合运算测试完成\n");
}

// 测试比较操作
TEST(SpaceComparisonTest, UnitComparisons) {
TEST_LOG("开始测试比较操作...");

TEST_LOG("测试相同单位比较");
logger::bytes b1(100), b2(200);
TEST_LOG("操作数: " << b1.count() << " B 与 " << b2.count() << " B");
EXPECT_TRUE(b1 < b2);
EXPECT_TRUE(b2 > b1);
EXPECT_FALSE(b1 == b2);
TEST_LOG("相同单位比较测试通过");

TEST_LOG("测试不同单位相等比较");
logger::kilobytes kb(1);
logger::bytes b3(1024);
TEST_LOG("操作数: " << kb.count() << " KB 与 " << b3.count() << " B");
EXPECT_TRUE(kb == b3);
TEST_LOG("不同单位相等比较测试通过");

TEST_LOG("测试不同单位大小比较");
logger::megabytes mb(1);
logger::kilobytes kb2(1023);
TEST_LOG("操作数: " << mb.count() << " MB 与 " << kb2.count() << " KB");
EXPECT_TRUE(mb > kb2);
TEST_LOG("不同单位大小比较测试通过");

TEST_LOG("测试跨单位比较");
logger::gigabytes gb(1);
logger::megabytes mb2(2048);
TEST_LOG("操作数: " << gb.count() << " GB 与 " << mb2.count() << " MB");
EXPECT_TRUE(gb < mb2);
TEST_LOG("跨单位比较测试通过");

TEST_LOG("比较操作测试完成\n");
}

// 测试特殊值(零值、负数)处理
TEST(SpaceSpecialValuesTest, ZeroAndNegative) {
TEST_LOG("开始测试特殊值处理...");

TEST_LOG("测试零值操作");
logger::bytes b_zero(0);
logger::kilobytes kb_zero(0);
TEST_LOG("操作数: " << b_zero.count() << " B 与 " << kb_zero.count() << " KB");
EXPECT_TRUE(b_zero == kb_zero);
TEST_LOG("零值操作测试通过");

TEST_LOG("测试负数处理(有符号类型)");
logger::space<int64_t> neg_space(-100);
logger::space<int64_t> pos_space(50);
TEST_LOG("操作数: 负数空间(" << neg_space.count() << ") 与 正数空间(" << pos_space.count() << ")");
EXPECT_TRUE(neg_space < pos_space);
TEST_LOG("负数比较测试通过");

TEST_LOG("测试负数加法");
auto neg_add = neg_space + pos_space;
TEST_LOG("操作: " << neg_space.count() << " + " << pos_space.count() << " = " << neg_add.count());
EXPECT_EQ(neg_add.count(), -50);
TEST_LOG("负数加法测试通过");

TEST_LOG("特殊值处理测试完成\n");
}

//int main(int argc, char **argv) {
//    std::cout << "\033[1;32m=== 开始运行 space.h 单元测试 ===\033[0m" << std::endl;
//    testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

