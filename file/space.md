# `space` 模块技术文档

## 1. 模块概述

`space` 模块是一个用于处理不同单位空间量（如字节、千字节等）的C++模板库，实现了类型安全的单位转换和算术运算功能。
该模块借鉴了C++标准库`<chrono>`中duration的设计思想，通过编译期计算确保类型安全和高效的单位转换。

## 2. 核心组件详解

### 2.1 `space` 类模板

#### 模板定义
```cpp
template<typename Rep_, typename Period_ = std::ratio<1>>
class space;
```

#### 模板参数
- `Rep_`：表示空间量的数值类型（如`int64_t`、`size_t`等）
- `Period_`：表示单位周期的`std::ratio`类型（默认`std::ratio<1>`，即基本单位）

#### 类成员

##### 类型定义
- `using rep = Rep_;`：数值类型别名
- `using period = Period_;`：周期类型别名

##### 构造函数
```cpp
constexpr space() : rep_() {}  /// 默认构造函数
constexpr space(const space &) noexcept = default;  /// 拷贝构造
constexpr space(space &&) noexcept = default;  /// 移动构造
```

##### 转换构造函数
```cpp
/// 从数值直接构造
template<typename Rep2_>
explicit constexpr space(const Rep2_ &r_) : rep_(r_) {}

/// 从其他space类型转换构造
template<typename Rep2_, typename Period2_>
constexpr space(const space<Rep2_, Period2_> &d_)
    : rep_(space_cast<space>(d_).count()) {}
    /// 这里的space_cast类似于std::chrono中的duratio,用于空间大小的转换
```

##### 成员函数
```cpp
constexpr Rep_ count() const { return rep_; }  /// 获取计数值
```

##### 算术运算符重载
```cpp
constexpr space operator+() const;  /// +
constexpr space operator-() const;  /// -
space &operator++();  /// 前置递增
space operator++(int);  /// 后置递增
space &operator--();  /// 前置递减
space operator--(int);  /// 后置递减
space &operator+=(const space &d_);  /// 复合加法
space &operator*=(const rep &rhs);  /// 复合乘法
space &operator-=(const space &d_);  /// 复合减法
space &operator/=(const rep &rhs);  /// 复合除法
space &operator%=(const rep &rhs);  /// 复合取模
```

### 2.2 `space_cast` 转换函数

#### 函数定义
```cpp
template<typename Tospace_, typename Rep_, typename Period_>
constexpr Tospace_ space_cast(const space<Rep_, Period_> &d);
```

#### 功能原理
- 实现不同单位空间量之间的安全转换
- 利用`std::ratio_divide`计算源周期与目标周期的比例关系
- 根据目标类型是否为浮点型选择不同的计算方式：
    - 浮点型：直接进行乘除运算
    - 整型：使用`CF::num / CF::den`计算比例因子

#### 类型别名解析
- `FromPeriod`：源空间周期类型
- `ToPeriod`：目标空间周期类型
- `CF`：周期比例因子（`std::ratio_divide<FromPeriod, ToPeriod>`）
- `CR`：目标空间数值类型

### 2.3 `higher_precision_space` 类型特性

#### 模板定义
```cpp
template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
struct higher_precision_space;
```

#### 功能原理
- 用于确定两个不同单位空间量中精度更高的类型
- 通过`std::ratio_less`比较周期大小：
    - 周期更小（`std::ratio_less<Period1_, Period2_>::value`为`true`）的类型精度更高
    - 周期相同时，统一数值类型为`std::common_type<Rep1_, Rep2_>::type`

#### 特化版本
- **不同周期情况**：
  ```cpp
  using type = typename std::conditional<
      std::ratio_less<Period1_, Period2_>::value,
      space<Rep1_, Period1_>,
      space<Rep2_, Period2_>>::type;
  ```
- **相同周期情况**：
  ```cpp
  using common_rep = typename std::common_type<Rep1_, Rep2_>::type;
  using type = space<common_rep, Period_>;
  ```

### 2.4 `unify_space_type` 类型统一工具

#### 模板定义
```cpp
template<typename Lhs, typename Rhs>
struct unify_space_type;
```

#### 功能原理
- 统一两个不同空间类型的表示
- 结合`higher_precision_space`确定共同周期
- 使用`std::common_type`确定共同数值类型
- 生成统一的`space`类型：
  ```cpp
  using Period = typename logger::higher_precision_space<...>::type::period;
  using Rep = typename std::common_type<...>::type;
  using type = logger::space<Rep, Period>;
  ```

## 3. 算术运算与比较操作

### 3.1 二元算术运算符

#### 加法运算符 `operator+`
```cpp
template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
constexpr auto operator+(const logger::space<Rep1_, Period1_> &lhs,
                         const logger::space<Rep2_, Period2_> &rhs);
```

#### 实现原理
- 使用`unify_space_type`确定统一类型`Common`
- 将左右操作数转换为`Common`类型后相加
- 返回统一类型的结果：
  ```cpp
  return Common(
      logger::space_cast<Common>(lhs).count() + logger::space_cast<Common>(rhs).count()
  );
  ```

#### 减法运算符 `operator-`
与加法原理类似，只是将加法运算改为减法运算。

### 3.2 除法与取模运算符

#### 除法运算符 `operator/`
```cpp
template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator/(const logger::space<Rep1, Period1> &lhs,
                         const logger::space<Rep2, Period2> &rhs);
```

#### 实现原理
- 确定共同周期`CommonPeriod`和共同数值类型`CommonRep`
- 将左右操作数转换为统一类型后进行除法运算
- 返回计算结果（非`space`类型，为数值类型）

#### 取模运算符 `operator%`
- 与加法类似，使用`unify_space_type`统一类型
- 执行取模运算并返回`space`类型结果

### 3.3 比较运算符

#### 相等运算符 `operator==`
```cpp
template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator==(const logger::space<Rep1, Period1> &lhs,
                          const logger::space<Rep2, Period2> &rhs);
```

#### 实现原理
- 统一为共同类型`Common`
- 比较转换后的计数值是否相等

#### 关系运算符 `<, >, <=, >=`
- 基于`operator<`实现其他关系运算符
- 统一类型后比较计数值大小

### 3.4 混合算术运算

#### 与数值类型的运算
```cpp
/// 仅允许T是算数类型（整型或者浮点类型）时，允许使用当前模板，如果不是算数类型，会在忽略这个模板，体现SFINAE的思想
template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr auto operator+(const logger::space<Rep_, Period_> &lhs, T rhs);
```

#### 实现原理
- 使用`std::common_type`确定共同数值类型
- 保持周期类型不变
- 执行算术运算并返回新的`space`对象

## 4. 单位定义与类型别名

### 4.1 存储单位常量
```cpp
constexpr int64_t KB = 1024LL;
constexpr int64_t MB = KB * 1024;
constexpr int64_t GB = MB * 1024;
constexpr int64_t TB = GB * 1024;
```

### 4.2 周期类型定义
```cpp
using kilo = std::ratio<KB, 1>;
using mega = std::ratio<MB, 1>;
using giga = std::ratio<GB, 1>;
using tera = std::ratio<TB, 1>;
```

### 4.3 空间类型别名
```cpp
using bytes = space<size_t>;                /// 字节（基本单位）
using kilobytes = space<int64_t, kilo>;     /// 千字节
using megabytes = space<int64_t, mega>;     /// 兆字节
using gigabytes = space<int64_t, giga>;     /// 吉字节
using terabytes = space<int64_t, tera>;     /// 太字节
```

## 5. 工作原理解析

### 5.1 编译期单位转换原理

`space_cast`函数利用`std::ratio_divide`在编译期计算周期比例，实现无运行时开销的单位转换：

```cpp
using CF = std::ratio_divide<FromPeriod, ToPeriod>;
/// 转换公式：目标值 = 源值 × (源周期 / 目标周期)
return Tospace_(d.count() * CF::num / CF::den);
```

### 5.2 类型安全的算术运算

所有算术运算都遵循以下步骤：
1. 确定参与运算的两个`space`类型的统一表示
2. 将操作数转换为统一类型
3. 执行算术运算
4. 返回统一类型的结果

这种方式确保了不同单位的空间量在运算时始终保持类型安全，避免了单位不匹配导致的逻辑错误。

### 5.3 精度处理策略

- **周期比较**：使用`std::ratio_less`在编译期比较周期大小，确保选择精度更高的单位
- **数值类型统一**：使用`std::common_type`获取最合适的数值类型，避免精度丢失
- **浮点型处理**：当目标类型为浮点型时，使用浮点运算保证精度

## 6. 使用示例

### 6.1 基本单位转换

```cpp
logger::kilobytes kb(1024);
logger::megabytes mb = logger::space_cast<logger::megabytes>(kb);
std::cout << mb.count() << " MB" << std::endl;  /// 输出: 1 MB
```

### 6.2 算术运算示例

```cpp
logger::gigabytes gb1(2);
logger::megabytes mb(1024);

/// 不同单位相加
logger::gigabytes total = gb1 + mb;
std::cout << total.count() << " GB" << std::endl;  /// 输出: 3 GB

/// 单位与数值运算
logger::terabytes tb = gb1 * 512;
std::cout << tb.count() << " TB" << std::endl;  /// 输出: 1 TB
```

### 6.3 比较操作示例

```cpp
logger::kilobytes kb1(1024);
logger::kilobytes kb2(2048);

if (kb1 < kb2) {
    std::cout << "kb1 is less than kb2" << std::endl;
}

logger::megabytes mb(1);
if (kb2 == mb) {
    std::cout << "kb2 equals mb" << std::endl;
}
```

## 7. 设计亮点与优势

1. **编译期计算**：所有单位转换和类型推导均在编译期完成，不产生运行时开销
2. **类型安全**：通过模板类型系统确保不同单位的空间量不会被错误地混合运算
3. **高精度支持**：自动选择精度更高的单位进行运算，减少精度损失
4. **灵活扩展**：可以轻松添加新的单位类型，只需定义对应的`std::ratio`周期
5. **符合习惯**：接口设计借鉴了C++标准库`<chrono>`，使用起来直观自然

## 8. 注意事项

1. **整数溢出风险**：在进行单位转换和算术运算时，需注意数值类型的范围，避免整数溢出
2. **浮点精度问题**：当使用浮点型数值时，可能存在精度损失问题
3. **单位语义**：确保在使用不同单位时，理解其实际语义（如这里的存储单位使用1024进制）
4. **性能考虑**：虽然编译期计算提高了运行时性能，但复杂的模板推导可能会增加编译时间

## 9. 未来改进方向

1. **添加乘法运算符返回单位乘积**：如`space * space`返回面积单位
2. **支持自定义单位**：提供更便捷的方式定义用户自定义单位
3. **增强错误处理**：在可能发生溢出或精度损失的操作中提供编译期警告
4. **添加序列化支持**：实现`space`类型的输入输出流操作符重载
5. **优化模板推导**：简化复杂运算中的模板参数推导过程
