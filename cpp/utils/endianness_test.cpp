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
    EXPECT_TRUE(IS_CONSTEXPR(ymn::get_cpu_endianness()));
}

TEST(endianness, is_swap_constexpr_1)
{
    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap((uint16_t)42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap((uint32_t)42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap((uint64_t)42)));
}

TEST(endianness, is_swap_constexpr_2)
{
    const uint16_t value1 = 42;
    const uint32_t value2 = 42;
    const uint64_t value3 = 42;

    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::swap(value3)));
}

TEST(endianness, is_swap_constexpr_3)
{
    uint16_t value1 = 42;
    uint32_t value2 = 42;
    uint64_t value3 = 42;

    EXPECT_FALSE(IS_CONSTEXPR(ymn::swap(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::swap(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::swap(value3)));
}

TEST(endianness, is_conversion_constexpr_1)
{
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe16(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole16(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be16tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le16tocpu(42)));

    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe32(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole32(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be32tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le32tocpu(42)));

    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe64(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole64(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be64tocpu(42)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le64tocpu(42)));
}

TEST(endianness, is_conversion_constexpr_2)
{
    const uint16_t value1 = 42;
    const uint32_t value2 = 42;
    const uint64_t value3 = 42;

    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe16(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole16(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be16tocpu(value1)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le16tocpu(value1)));

    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe32(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole32(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be32tocpu(value2)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le32tocpu(value2)));

    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputobe64(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::cputole64(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::be64tocpu(value3)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::le64tocpu(value3)));
}

TEST(endianness, is_conversion_constexpr_3)
{
    uint16_t value1 = 42;
    uint32_t value2 = 42;
    uint64_t value3 = 42;

    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputobe16(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputole16(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::be16tocpu(value1)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::le16tocpu(value1)));

    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputobe32(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputole32(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::be32tocpu(value2)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::le32tocpu(value2)));

    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputobe64(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::cputole64(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::be64tocpu(value3)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::le64tocpu(value3)));
}

TEST(endianness, swap)
{
    EXPECT_EQ(0x1234U, ymn::swap((uint16_t)0x3412U));
    EXPECT_EQ(0x3412U, ymn::swap((uint16_t)0x1234U));

    EXPECT_EQ(0x12345678U, ymn::swap((uint32_t)0x78563412U));
    EXPECT_EQ(0x78563412U, ymn::swap((uint32_t)0x12345678U));

    EXPECT_EQ(0x1234567890ABCDEFULL, ymn::swap((uint64_t)0xEFCDAB9078563412ULL));
    EXPECT_EQ(0xEFCDAB9078563412ULL, ymn::swap((uint64_t)0x1234567890ABCDEFULL));
}

TEST(endianness, cputobe16)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234U, ymn::cputobe16(0x1234U));
    else
        EXPECT_EQ(0x3412U, ymn::cputobe16(0x1234U));
}

TEST(endianness, cputole16)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234U, ymn::cputole16(0x1234U));
    else
        EXPECT_EQ(0x3412U, ymn::cputole16(0x1234U));
}

TEST(endianness, be16tocpu)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234U, ymn::be16tocpu(0x1234U));
    else
        EXPECT_EQ(0x3412U, ymn::be16tocpu(0x1234U));
}

TEST(endianness, le16tocpu)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234U, ymn::le16tocpu(0x1234U));
    else
        EXPECT_EQ(0x3412U, ymn::le16tocpu(0x1234U));
}

TEST(endianness, cputobe32)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, ymn::cputobe32(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, ymn::cputobe32(0x12345678U));
}

TEST(endianness, cputole32)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, ymn::cputole32(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, ymn::cputole32(0x12345678U));
}

TEST(endianness, be32tocpu)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, ymn::be32tocpu(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, ymn::be32tocpu(0x12345678U));
}

TEST(endianness, le32tocpu)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x12345678U, ymn::le32tocpu(0x12345678U));
    else
        EXPECT_EQ(0x78563412U, ymn::le32tocpu(0x12345678U));
}

TEST(endianness, cputobe64)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, ymn::cputobe64(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, ymn::cputobe64(0x1234567890ABCDEFULL));
}

TEST(endianness, cputole64)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, ymn::cputole64(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, ymn::cputole64(0x1234567890ABCDEFULL));
}

TEST(endianness, be64tocpu)
{
    if (CPU_BIG_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, ymn::be64tocpu(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, ymn::be64tocpu(0x1234567890ABCDEFULL));
}

TEST(endianness, le64tocpu)
{
    if (CPU_LITTLE_ENDIAN == ymn::get_cpu_endianness())
        EXPECT_EQ(0x1234567890ABCDEFULL, ymn::le64tocpu(0x1234567890ABCDEFULL));
    else
        EXPECT_EQ(0xEFCDAB9078563412ULL, ymn::le64tocpu(0x1234567890ABCDEFULL));
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
