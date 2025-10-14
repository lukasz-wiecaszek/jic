/**
 * @file endianness_test.cpp
 *
 * Test procedures for various conversions from host byte order to
 * little or big endian byte order.
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

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "endianness.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
/* Because the noexcept operator always returns true for a constant expression,
it can be used to check if a particular invocation of a constexpr
takes the constant expression branch. */
#define IS_CONSTEXPR(expr) noexcept(expr)

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
namespace
{

TEST(endianness, is_endianness_detection_constexpr)
{
    EXPECT_TRUE(IS_CONSTEXPR(lts::get_cpu_endianness()));
}

TEST(endianness, is_swap_constexpr_1)
{
    EXPECT_TRUE(IS_CONSTEXPR(lts::swap((uint16_t)42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::swap((uint32_t)42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::swap((uint64_t)42)));
}

TEST(endianness, is_swap_constexpr_2)
{
    const uint16_t value1 = 42;
    const uint32_t value2 = 42;
    const uint64_t value3 = 42;

    EXPECT_TRUE(IS_CONSTEXPR(lts::swap(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::swap(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::swap(value3)));
}

TEST(endianness, is_swap_constexpr_3)
{
    uint16_t value1 = 42;
    uint32_t value2 = 42;
    uint64_t value3 = 42;

    EXPECT_FALSE(IS_CONSTEXPR(lts::swap(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::swap(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::swap(value3)));
}

TEST(endianness, is_conversion_constexpr_1)
{
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe16(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole16(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be16tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le16tocpu(42)));

    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe32(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole32(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be32tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le32tocpu(42)));

    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe64(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole64(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be64tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le64tocpu(42)));
}

TEST(endianness, is_conversion_constexpr_2)
{
    const uint16_t value1 = 42;
    const uint32_t value2 = 42;
    const uint64_t value3 = 42;

    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe16(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole16(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be16tocpu(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le16tocpu(value1)));

    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe32(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole32(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be32tocpu(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le32tocpu(value2)));

    EXPECT_TRUE(IS_CONSTEXPR(lts::cputobe64(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::cputole64(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::be64tocpu(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(lts::le64tocpu(value3)));
}

TEST(endianness, is_conversion_constexpr_3)
{
    uint16_t value1 = 42;
    uint32_t value2 = 42;
    uint64_t value3 = 42;

    EXPECT_FALSE(IS_CONSTEXPR(lts::cputobe16(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::cputole16(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::be16tocpu(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::le16tocpu(value1)));

    EXPECT_FALSE(IS_CONSTEXPR(lts::cputobe32(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::cputole32(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::be32tocpu(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::le32tocpu(value2)));

    EXPECT_FALSE(IS_CONSTEXPR(lts::cputobe64(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::cputole64(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::be64tocpu(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(lts::le64tocpu(value3)));
}

TEST(endianness, swap)
{
    EXPECT_EQ(0x1234U, lts::swap((uint16_t)0x3412U));
    EXPECT_EQ(0x3412U, lts::swap((uint16_t)0x1234U));

    EXPECT_EQ(0x12345678U, lts::swap((uint32_t)0x78563412U));
    EXPECT_EQ(0x78563412U, lts::swap((uint32_t)0x12345678U));

    EXPECT_EQ(0x1234567890ABCDEFULL, lts::swap((uint64_t)0xEFCDAB9078563412ULL));
    EXPECT_EQ(0xEFCDAB9078563412ULL, lts::swap((uint64_t)0x1234567890ABCDEFULL));
}

TEST(endianness, cputobe16)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234U, lts::cputobe16(0x1234U));
    else
        EXPECT_EQ(0x3412U, lts::cputobe16(0x1234U));
}

TEST(endianness, cputole16)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234U, lts::cputole16(0x1234U));
    else
        EXPECT_EQ(0x3412U, lts::cputole16(0x1234U));
}

TEST(endianness, be16tocpu)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234U, lts::be16tocpu(0x1234U));
    else
        EXPECT_EQ(0x3412U, lts::be16tocpu(0x1234U));
}

TEST(endianness, le16tocpu)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234U, lts::le16tocpu(0x1234U));
    else
        EXPECT_EQ(0x3412U, lts::le16tocpu(0x1234U));
}

TEST(endianness, cputobe32)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, lts::cputobe32(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, lts::cputobe32(0x12345678U));
}

TEST(endianness, cputole32)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, lts::cputole32(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, lts::cputole32(0x12345678U));
}

TEST(endianness, be32tocpu)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, lts::be32tocpu(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, lts::be32tocpu(0x12345678U));
}

TEST(endianness, le32tocpu)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, lts::le32tocpu(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, lts::le32tocpu(0x12345678U));
}

TEST(endianness, cputobe64)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, lts::cputobe64(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, lts::cputobe64(0x1234567890ABCDEFULL));
}

TEST(endianness, cputole64)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, lts::cputole64(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, lts::cputole64(0x1234567890ABCDEFULL));
}

TEST(endianness, be64tocpu)
{
    if (CPU_BIG_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, lts::be64tocpu(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, lts::be64tocpu(0x1234567890ABCDEFULL));
}

TEST(endianness, le64tocpu)
{
    if (CPU_LITTLE_ENDIAN == lts::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, lts::le64tocpu(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, lts::le64tocpu(0x1234567890ABCDEFULL));
}

} // end of anonymous namespace

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/
