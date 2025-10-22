/**
 * @file endianness.hpp
 *
 * This header file defines set of functions responsible for converting
 * byte encoding scheme of integer values (uint16_t, uint32_t, uint64_t).
 * Such conversions can be performed from byte order applicable on
 * current CPU (host) to the little-endian or big-endian byte order.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef _ENDIANNESS_HPP_
#define _ENDIANNESS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cinttypes>
#include <type_traits>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
#if __cplusplus >= 202002L
#include <bit>
namespace lts
{
    using endian = std::endian;
} /* end of namespace lts */
#else
namespace lts
{
#if defined(__BYTE_ORDER)
    enum class endian
    {
        little = __LITTLE_ENDIAN,
        big    = __BIG_ENDIAN,
        native = __BYTE_ORDER
    };
#else
    enum class endian
    {
        little = __ORDER_LITTLE_ENDIAN__,
        big    = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
    };
#endif
} /* end of namespace lts */
#endif

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{
#if __cplusplus >= 202300L
constexpr uint16_t byteswap(uint16_t x)
{
    return std::byteswap(x);
}

constexpr uint32_t byteswap(uint32_t x)
{
    return std::byteswap(x);
}

constexpr uint64_t byteswap(uint64_t x)
{
    return std::byteswap(x);
}
#else
constexpr uint16_t byteswap(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

constexpr uint32_t byteswap(uint32_t x)
{
    x = ((x << 8) & 0xFF00FF00U) | ((x >> 8) & 0x00FF00FFU);
    return (x << 16) | (x >> 16);
}

constexpr uint64_t byteswap(uint64_t x)
{
    x = ((x <<  8) & 0xFF00FF00FF00FF00ULL) | ((x >>  8) & 0x00FF00FF00FF00FFULL);
    x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
    return (x << 32) | (x >> 32);
}
#endif

template<typename T>
constexpr T cputobe(T x)
{
    if constexpr (endian::native == endian::big)
        return x;
    else
    if constexpr (endian::native == endian::little)
        return byteswap(std::make_unsigned_t<T>(x));
    else
        static_assert((endian::native == endian::big) || (endian::native == endian::little),
            "endian::native is neither big nor little!");
}

template<typename T>
constexpr T cputole(T x)
{
    if constexpr (endian::native == endian::big)
        return byteswap(std::make_unsigned_t<T>(x));
    else
    if constexpr (endian::native == endian::little)
        return x;
    else
        static_assert((endian::native == endian::big) || (endian::native == endian::little),
            "endian::native is neither big nor little!");
}

template<typename T>
constexpr T betocpu(T x)
{
    if constexpr (endian::native == endian::big)
        return x;
    else
    if constexpr (endian::native == endian::little)
        return byteswap(std::make_unsigned_t<T>(x));
    else
        static_assert((endian::native == endian::big) || (endian::native == endian::little),
            "endian::native is neither big nor little!");
}

template<typename T>
constexpr T letocpu(T x)
{
    if constexpr (endian::native == endian::big)
        return byteswap(std::make_unsigned_t<T>(x));
    else
    if constexpr (endian::native == endian::little)
        return x;
    else
        static_assert((endian::native == endian::big) || (endian::native == endian::little),
            "endian::native is neither big nor little!");
}

} /* end of namespace lts */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

#endif /* _ENDIANNESS_HPP_ */
