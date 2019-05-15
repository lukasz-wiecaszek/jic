/**
 * @file strtointeger_test.cpp
 *
 * Test procedures for strtointeger() function.
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
#include <cstdint>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "strtointeger.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define EXPECT_SUCCESS(str, in, out) \
    do { \
        EXPECT_EQ(ymn::strtointeger_conversion_status_e::success, ymn::strtointeger(str, out)); \
        EXPECT_EQ(in, out); \
    } while (0)

#define EXPECT_INVALID_ARGUMENT(str, out) \
    do { \
        EXPECT_EQ(ymn::strtointeger_conversion_status_e::invalid_argument, ymn::strtointeger(str, out)); \
    } while (0)

#define EXPECT_OUT_OF_RANGE(str, out) \
    do { \
        EXPECT_EQ(ymn::strtointeger_conversion_status_e::out_of_range, ymn::strtointeger(str, out)); \
    } while (0)

#define EXPECT_CONVERSION_ERROR(str, out) \
    do { \
        EXPECT_EQ(ymn::strtointeger_conversion_status_e::conversion_error, ymn::strtointeger(str, out)); \
    } while (0)

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

TEST(strtointeger, int8_t)
{
    int8_t s8_value;

    EXPECT_SUCCESS("-1", -1, s8_value);
    EXPECT_SUCCESS(" 0",  0, s8_value);
    EXPECT_SUCCESS(" 1", +1, s8_value);

    EXPECT_SUCCESS("-128", -128, s8_value);
    EXPECT_SUCCESS("+127", +127, s8_value);

    EXPECT_OUT_OF_RANGE("-129", s8_value);
    EXPECT_OUT_OF_RANGE("+128", s8_value);
}

TEST(strtointeger, uint8_t)
{
    uint8_t u8_value;

    EXPECT_SUCCESS(" 0",  0, u8_value);
    EXPECT_SUCCESS("+1", +1, u8_value);

    EXPECT_SUCCESS("+255", +255, u8_value);
    EXPECT_OUT_OF_RANGE("+256", u8_value);
}

TEST(strtointeger, int16_t)
{
    int16_t s16_value;

    EXPECT_SUCCESS("-1", -1, s16_value);
    EXPECT_SUCCESS(" 0",  0, s16_value);
    EXPECT_SUCCESS("+1", +1, s16_value);

    EXPECT_SUCCESS("-32768", -32768, s16_value);
    EXPECT_SUCCESS("+32767", +32767, s16_value);

    EXPECT_OUT_OF_RANGE("-32769", s16_value);
    EXPECT_OUT_OF_RANGE("+32768", s16_value);
}

TEST(strtointeger, uint16_t)
{
    uint16_t u16_value;

    EXPECT_SUCCESS(" 0",  0, u16_value);
    EXPECT_SUCCESS("+1", +1, u16_value);

    EXPECT_SUCCESS("+65535", +65535, u16_value);
    EXPECT_OUT_OF_RANGE("+65536", u16_value);
}

TEST(strtointeger, int32_t)
{
    int32_t s32_value;

    EXPECT_SUCCESS("-1", -1, s32_value);
    EXPECT_SUCCESS(" 0",  0, s32_value);
    EXPECT_SUCCESS("+1", +1, s32_value);

    EXPECT_SUCCESS("-2147483648", -2147483648, s32_value);
    EXPECT_SUCCESS("+2147483647", +2147483647, s32_value);

    EXPECT_OUT_OF_RANGE("-2147483649", s32_value);
    EXPECT_OUT_OF_RANGE("+2147483648", s32_value);
}

TEST(strtointeger, uint32_t)
{
    uint32_t u32_value;

    EXPECT_SUCCESS(" 0",  0, u32_value);
    EXPECT_SUCCESS("+1", +1, u32_value);

    EXPECT_SUCCESS("+4294967295", +4294967295, u32_value);
    EXPECT_OUT_OF_RANGE("+4294967296", u32_value);
}

TEST(strtointeger, int64_t)
{
    int64_t s64_value;

    EXPECT_SUCCESS("-1", -1, s64_value);
    EXPECT_SUCCESS(" 0",  0, s64_value);
    EXPECT_SUCCESS("+1", +1, s64_value);

    EXPECT_SUCCESS("-9223372036854775808", -9223372036854775808LL, s64_value);
    EXPECT_SUCCESS("+9223372036854775807", +9223372036854775807LL, s64_value);

    EXPECT_OUT_OF_RANGE("-9223372036854775809", s64_value);
    EXPECT_OUT_OF_RANGE("+9223372036854775808", s64_value);
}

TEST(strtointeger, uint64_t)
{
    uint64_t u64_value;

    EXPECT_SUCCESS(" 0",  0, u64_value);
    EXPECT_SUCCESS("+1", +1, u64_value);

    EXPECT_SUCCESS("+18446744073709551615", +18446744073709551615ULL, u64_value);
    EXPECT_OUT_OF_RANGE("+18446744073709551616", u64_value);
}

TEST(strtointeger, invalid_argument)
{
    int value;

    EXPECT_INVALID_ARGUMENT("this shall give invalid argument", value);
    EXPECT_INVALID_ARGUMENT("01abc", value);
    EXPECT_INVALID_ARGUMENT("0xdaabOabc", value);
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
