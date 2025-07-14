/**
 * @file      space.h
 * @brief     [用于空间量的转换，类似于std::chrono]
 * @author    Weijh
 * @version   1.0
 */
#pragma once

#include <ratio>
#include <iostream>

namespace logger {

    template<class Rep_, class Period_ = std::ratio<1>>
    class space;

    /// 用于将一个时间段从一个周期单位转换到另一个周期单位
    template<typename Tospace_, typename Rep_, typename Period_>
    constexpr Tospace_ space_cast(const space<Rep_, Period_> &d) {
        using FromPeriod = Period_;
        using ToPeriod = typename Tospace_::period;
        using CF = std::ratio_divide<FromPeriod, ToPeriod>;

        using CR = typename Tospace_::rep;
        if constexpr (std::is_floating_point_v<CR>) {
            return Tospace_(static_cast<CR>(d.count()) * static_cast<CR>(CF::num) / static_cast<CR>(CF::den));
        } else {
            /// 可以扩展使用更安全的乘法处理
            return Tospace_(static_cast<CR>(d.count()) * CF::num / CF::den);
        }
    }


    /// space实现
    template<typename Rep_, typename Period_>
    class space {
    public:
        using rep = Rep_;
        using period = Period_;

        constexpr space()
                : rep_() {}

        constexpr space(const space &) noexcept = default;

        constexpr space(space &&) noexcept = default;

        constexpr space &operator=(const space &) noexcept = default;

        constexpr space &operator=(space &&) noexcept = default;


        template<typename Rep2_>
        explicit constexpr space(const Rep2_ &r_)
                : rep_(r_) {}

        template<typename Rep2_, typename Period2_>
        constexpr space(const space<Rep2_, Period2_> &d_)
                : rep_(space_cast<space>(d_).count()) {}

        constexpr Rep_ count() const {
            return rep_;
        }


        /// operator+ / operator-：将两个不同周期单位的 space 转换成共同单位后再相加/减
        constexpr space operator+() const {
            return *this;
        }

        constexpr space operator-() const {
            return space(-count());
        }

        space &operator++() {
            ++rep_;
            return *this;
        }

        space operator++(int) {
            space temp_(*this);
            ++*this;
            return temp_;
        }

        space &operator--() {
            --rep_;
            return *this;
        }

        space operator--(int) {
            space temp_(*this);
            --*this;
            return temp_;
        }

        space &operator+=(const space &d_) {
            rep_ += d_.count();
            return *this;
        }

        space &operator*=(const rep &rhs) {
            rep_ *= rhs;
            return *this;
        }

        space &operator-=(const space &d_) {
            rep_ -= d_.count();
            return *this;
        }

        space &operator/=(const rep &rhs) {
            rep_ /= rhs;
            return *this;
        }

        space &operator%=(const rep &rhs) {
            rep_ %= rhs;
            return *this;
        }

    private:
        Rep_ rep_;
    };

    /// 在两个 space 类型中选取精度更高的单位，这里是处理两个不同周期的情况
    template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
    struct higher_precision_space {
        using type = typename std::conditional<
                std::ratio_less<Period1_, Period2_>::value,
                space<Rep1_, Period1_>,
                space<Rep2_, Period2_>>::type;

    };

    /// 这里是周期相同时的情况，统一Rep类型
    template <typename Rep1_, typename Period_, typename Rep2_>
    struct higher_precision_space<Rep1_, Period_, Rep2_, Period_> {
        /// 取Rep1_和Rep2_的共同类型（如int和long的共同类型是long)
        using common_rep = typename std::common_type<Rep1_, Rep2_>::type;
        /// 周期不变Rep统一为共同类型
        using type = space<common_rep, Period_>;
    };

    constexpr int64_t KB = 1024LL;
    constexpr int64_t MB = KB * 1024;
    constexpr int64_t GB = MB * 1024;
    constexpr int64_t TB = GB * 1024;

    using kilo = std::ratio<KB, 1>;
    using mega = std::ratio<MB, 1>;
    using giga = std::ratio<GB, 1>;
    using tera = std::ratio<TB, 1>;


    using bytes = space<size_t>;
    using kilobytes = space<int64_t, kilo>;
    using megabytes = space<int64_t, mega>;
    using gigabytes = space<int64_t, giga>;
    using terabytes = space<int64_t, tera>;

}

template<typename Lhs, typename Rhs>
struct unify_space_type {
    using Period = typename logger::higher_precision_space<
            typename Lhs::rep, typename Lhs::period,
            typename Rhs::rep, typename Rhs::period>::type::period;

    using Rep = typename std::common_type<typename Lhs::rep, typename Rhs::rep>::type;
    using type = logger::space<Rep, Period>;
};

/**
 * 找出两个 space 类型中精度更高（周期更小）的类型；
 * 把两个 space 都转换成该更高精度类型后再执行加减法；
 * 最终构造并返回该统一类型的结果。
 * */
template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
constexpr auto operator+(const logger::space<Rep1_, Period1_> &lhs,
                         const logger::space<Rep2_, Period2_> &rhs) {
    using Common = typename unify_space_type<logger::space<Rep1_, Period1_>, logger::space<Rep2_, Period2_>>::type;
    return Common(
            logger::space_cast<Common>(lhs).count() + logger::space_cast<Common>(rhs).count()
    );
}


template<typename Rep1_, typename Period1_, typename Rep2_, typename Period2_>
constexpr auto operator-(const logger::space<Rep1_, Period1_> &lhs,
                         const logger::space<Rep2_, Period2_> &rhs) {
    using Common = typename unify_space_type<logger::space<Rep1_, Period1_>, logger::space<Rep2_, Period2_>>::type;
    return Common(
            logger::space_cast<Common>(lhs).count() - logger::space_cast<Common>(rhs).count()
    );
}

/// / 操作符先统一单位，再做进一步运算
template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator/(const logger::space<Rep1, Period1> &lhs,
                         const logger::space<Rep2, Period2> &rhs) {
    using CommonPeriod = typename logger::higher_precision_space<Rep1, Period1, Rep2, Period2>::type::period;
    using CommonRep = typename std::common_type<Rep1, Rep2, double>::type;
    using CommonSpace = logger::space<CommonRep, CommonPeriod>;

    CommonRep lhs_val = logger::space_cast<CommonSpace>(lhs).count();
    CommonRep rhs_val = logger::space_cast<CommonSpace>(rhs).count();

    return lhs_val / rhs_val;
}


template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator%(const logger::space<Rep1, Period1> &lhs,
                         const logger::space<Rep2, Period2> &rhs) {
    using Common = typename unify_space_type<
            logger::space<Rep1, Period1>,
            logger::space<Rep2, Period2>>::type;

    using CT = typename Common::rep;
    CT lhs_val = logger::space_cast<Common>(lhs).count();
    CT rhs_val = logger::space_cast<Common>(rhs).count();

    return Common(lhs_val % rhs_val);
}


template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr bool operator==(const logger::space<Rep1, Period1> &lhs,
                          const logger::space<Rep2, Period2> &rhs) {
    using Common = typename unify_space_type<
            logger::space<Rep1, Period1>, logger::space<Rep2, Period2>>::type;
    return logger::space_cast<Common>(lhs).count() == logger::space_cast<Common>(rhs).count();
}

/// 提供space + int / int + space
template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr auto operator+(const logger::space<Rep_, Period_> &lhs, T rhs) {
    using CommonRep = typename std::common_type<Rep_, T>::type;
    using Result = logger::space<CommonRep, Period_>;
    return Result(lhs.count() + rhs);
}

template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr auto operator+(T lhs, const logger::space<Rep_, Period_> &rhs) {
    using CommonRep = typename std::common_type<Rep_, T>::type;
    using Result = logger::space<CommonRep, Period_>;
    return Result(lhs + rhs.count());
}

template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr auto operator*(const logger::space<Rep_, Period_> &lhs, T rhs) {
    using CommonRep = typename std::common_type<Rep_, T>::type;
    using Result = logger::space<CommonRep, Period_>;
    return Result(lhs.count() * rhs);
}

template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, logger::space<Rep_, Period_>>, int> = 0>
constexpr auto operator*(T lhs, const logger::space<Rep_, Period_> &rhs) {
    return rhs * lhs; /// 利用已定义的 operator*
}

template<typename Rep_, typename Period_, typename T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
constexpr auto operator/(const logger::space<Rep_, Period_> &lhs, T rhs) {
    using CommonRep = typename std::common_type<Rep_, T>::type;
    using Result = logger::space<CommonRep, Period_>;
    return Result(lhs.count() / rhs);
}

/// < > <= >=
template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator<(const logger::space<Rep1, Period1>& lhs,
                         const logger::space<Rep2, Period2>& rhs) {
    using CommomRep = typename unify_space_type<
            logger::space<Rep1, Period1>, logger::space<Rep2, Period2>>::type;
    return logger::space_cast<CommomRep>(lhs).count() < logger::space_cast<CommomRep>(rhs).count();
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator>(const logger::space<Rep1, Period1>& lhs,
                         const logger::space<Rep2, Period2>& rhs) {
    return rhs < lhs;
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator<=(const logger::space<Rep1, Period1>& lhs,
                          const logger::space<Rep2, Period2>& rhs) {
    return !(lhs > rhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator>=(const logger::space<Rep1, Period1>& lhs,
                          const logger::space<Rep2, Period2>& rhs) {
    return !(lhs < rhs);
}

template<typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator!=(const logger::space<Rep1, Period1>& lhs,
                          const logger::space<Rep2, Period2>& rhs) {
    return !(lhs == rhs);
}





