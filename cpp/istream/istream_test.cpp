/**
 * @file istream_test.cpp
 *
 * Test procedures for 'istream' type.
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
#include "istream.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

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

TEST(istream, create_1_onstack)
{
    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(nullptr, 0);
    EXPECT_FALSE(stream_be.is_valid());

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(nullptr, 0);
    EXPECT_FALSE(stream_le.is_valid());

    std::cout << (std::string)stream_be << std::endl;
    std::cout << (std::string)stream_le << std::endl;
}

TEST(istream, create_1_onheap)
{
    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> *stream_be =
        new lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN>(nullptr, 0);
    EXPECT_FALSE(stream_be->is_valid());

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> *stream_le =
        new lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN>(nullptr, 0);
    EXPECT_FALSE(stream_le->is_valid());

    std::cout << (std::string)*stream_be << std::endl;
    std::cout << (std::string)*stream_le << std::endl;

    delete stream_le; /* that will reach the second branch of the destructor */
    delete stream_be; /* that will reach the second branch of the destructor */
}

TEST(istream, create_2)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    std::cout << (std::string)stream_be << std::endl;
    std::cout << (std::string)stream_le << std::endl;
}

TEST(istream, create_3)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream1_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream1_be.is_valid());

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream2_be(stream1_be);
    EXPECT_TRUE(stream2_be.is_valid());

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream1_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream1_le.is_valid());

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream2_le(stream1_le);
    EXPECT_TRUE(stream2_le.is_valid());

    std::cout << (std::string)stream1_be << std::endl;
    std::cout << (std::string)stream1_le << std::endl;
}

#include "read_peek_u8.test"
#include "read_peek_s8.test"

#include "read_peek_u16.test"
#include "read_peek_s16.test"

#include "read_peek_u32.test"
#include "read_peek_s32.test"

#include "read_peek_u64.test"
#include "read_peek_s64.test"

#define READ_BITS_TEST(stream, bits, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_bits(bits, value));                                         \
    EXPECT_EQ(expected_bytepos, stream.tell());                                         \
    EXPECT_EQ(expected_bitpos, stream.tell_bits());                                     \
    EXPECT_EQ(expected_value, value);                                                   \
    std::cout << (std::string)stream << std::endl;

TEST(istream, read_bits_be)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_EQ(0, stream_be.tell());
    EXPECT_EQ(0, stream_be.tell_bits());

    READ_BITS_TEST(stream_be, 1, 0, 1, 0);
    READ_BITS_TEST(stream_be, 1, 0, 2, 1);
    READ_BITS_TEST(stream_be, 1, 0, 3, 0);
    READ_BITS_TEST(stream_be, 1, 0, 4, 1);
    READ_BITS_TEST(stream_be, 1, 0, 5, 0);
    READ_BITS_TEST(stream_be, 1, 0, 6, 1);
    READ_BITS_TEST(stream_be, 1, 0, 7, 0);
    READ_BITS_TEST(stream_be, 1, 1, 0, 1);
    READ_BITS_TEST(stream_be, 1, 1, 1, 0);
    READ_BITS_TEST(stream_be, 1, 1, 2, 1);

    READ_BITS_TEST(stream_be, 2,   1, 4, 0b01);
    READ_BITS_TEST(stream_be, 3,   1, 7, 0b010);
    READ_BITS_TEST(stream_be, 4,   2, 3, 0b1010);
    READ_BITS_TEST(stream_be, 5,   3, 0, 0b10101);
    READ_BITS_TEST(stream_be, 6,   3, 6, 0b010101);
    READ_BITS_TEST(stream_be, 7,   4, 5, 0b0101010);
    READ_BITS_TEST(stream_be, 8,   5, 5, 0b10101010);
    READ_BITS_TEST(stream_be, 9,   6, 6, 0b101010101);
    READ_BITS_TEST(stream_be, 10,  8, 0, 0b0101010101);
    READ_BITS_TEST(stream_be, 11,  9, 3, 0b01010101010);
    READ_BITS_TEST(stream_be, 12, 10, 7, 0b101010101010);
    READ_BITS_TEST(stream_be, 13, 12, 4, 0b1010101010101);
    READ_BITS_TEST(stream_be, 14, 14, 2, 0b01010101010101);

    stream_be.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(14, 2), before);
            EXPECT_EQ(lts::istream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_bits(15, value));

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, read_bits_le)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_EQ(0, stream_le.tell());
    EXPECT_EQ(0, stream_le.tell_bits());

    READ_BITS_TEST(stream_le, 1, 0, 1, 0);
    READ_BITS_TEST(stream_le, 1, 0, 2, 1);
    READ_BITS_TEST(stream_le, 1, 0, 3, 0);
    READ_BITS_TEST(stream_le, 1, 0, 4, 1);
    READ_BITS_TEST(stream_le, 1, 0, 5, 0);
    READ_BITS_TEST(stream_le, 1, 0, 6, 1);
    READ_BITS_TEST(stream_le, 1, 0, 7, 0);
    READ_BITS_TEST(stream_le, 1, 1, 0, 1);
    READ_BITS_TEST(stream_le, 1, 1, 1, 0);
    READ_BITS_TEST(stream_le, 1, 1, 2, 1);

    READ_BITS_TEST(stream_le, 2,   1, 4, 0b01);
    READ_BITS_TEST(stream_le, 3,   1, 7, 0b010);
    READ_BITS_TEST(stream_le, 4,   2, 3, 0b1010);
    READ_BITS_TEST(stream_le, 5,   3, 0, 0b10101);
    READ_BITS_TEST(stream_le, 6,   3, 6, 0b010101);
    READ_BITS_TEST(stream_le, 7,   4, 5, 0b0101010);
    READ_BITS_TEST(stream_le, 8,   5, 5, 0b10101010);
    READ_BITS_TEST(stream_le, 9,   6, 6, 0b101010101);
    READ_BITS_TEST(stream_le, 10,  8, 0, 0b0101010101);
    READ_BITS_TEST(stream_le, 11,  9, 3, 0b01010101010);
    READ_BITS_TEST(stream_le, 12, 10, 7, 0b101010101010);
    READ_BITS_TEST(stream_le, 13, 12, 4, 0b1010101010101);
    READ_BITS_TEST(stream_le, 14, 14, 2, 0b01010101010101);

    stream_le.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(14, 2), before);
            EXPECT_EQ(lts::istream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_bits(15, value));

    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_BITS_TEST(stream, bits, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    stream.skip_bits(bits);                                                             \
    EXPECT_EQ(expected_bytepos, stream.tell());                                         \
    EXPECT_EQ(expected_bitpos, stream.tell_bits());                                     \
    EXPECT_EQ(expected_value, value);                                                   \
    std::cout << (std::string)stream << std::endl;

TEST(istream, peek_bits_be)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_EQ(0, stream_be.tell());
    EXPECT_EQ(0, stream_be.tell_bits());

    PEEK_BITS_TEST(stream_be, 1, 0, 1, 0);
    PEEK_BITS_TEST(stream_be, 1, 0, 2, 1);
    PEEK_BITS_TEST(stream_be, 1, 0, 3, 0);
    PEEK_BITS_TEST(stream_be, 1, 0, 4, 1);
    PEEK_BITS_TEST(stream_be, 1, 0, 5, 0);
    PEEK_BITS_TEST(stream_be, 1, 0, 6, 1);
    PEEK_BITS_TEST(stream_be, 1, 0, 7, 0);
    PEEK_BITS_TEST(stream_be, 1, 1, 0, 1);
    PEEK_BITS_TEST(stream_be, 1, 1, 1, 0);
    PEEK_BITS_TEST(stream_be, 1, 1, 2, 1);

    PEEK_BITS_TEST(stream_be, 2,   1, 4, 0b01);
    PEEK_BITS_TEST(stream_be, 3,   1, 7, 0b010);
    PEEK_BITS_TEST(stream_be, 4,   2, 3, 0b1010);
    PEEK_BITS_TEST(stream_be, 5,   3, 0, 0b10101);
    PEEK_BITS_TEST(stream_be, 6,   3, 6, 0b010101);
    PEEK_BITS_TEST(stream_be, 7,   4, 5, 0b0101010);
    PEEK_BITS_TEST(stream_be, 8,   5, 5, 0b10101010);
    PEEK_BITS_TEST(stream_be, 9,   6, 6, 0b101010101);
    PEEK_BITS_TEST(stream_be, 10,  8, 0, 0b0101010101);
    PEEK_BITS_TEST(stream_be, 11,  9, 3, 0b01010101010);
    PEEK_BITS_TEST(stream_be, 12, 10, 7, 0b101010101010);
    PEEK_BITS_TEST(stream_be, 13, 12, 4, 0b1010101010101);
    PEEK_BITS_TEST(stream_be, 14, 14, 2, 0b01010101010101);

    stream_be.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(14, 2), before);
            EXPECT_EQ(lts::istream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.peek_bits(15, value));

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, peek_bits_le)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_EQ(0, stream_le.tell());
    EXPECT_EQ(0, stream_le.tell_bits());

    PEEK_BITS_TEST(stream_le, 1, 0, 1, 0);
    PEEK_BITS_TEST(stream_le, 1, 0, 2, 1);
    PEEK_BITS_TEST(stream_le, 1, 0, 3, 0);
    PEEK_BITS_TEST(stream_le, 1, 0, 4, 1);
    PEEK_BITS_TEST(stream_le, 1, 0, 5, 0);
    PEEK_BITS_TEST(stream_le, 1, 0, 6, 1);
    PEEK_BITS_TEST(stream_le, 1, 0, 7, 0);
    PEEK_BITS_TEST(stream_le, 1, 1, 0, 1);
    PEEK_BITS_TEST(stream_le, 1, 1, 1, 0);
    PEEK_BITS_TEST(stream_le, 1, 1, 2, 1);

    PEEK_BITS_TEST(stream_le, 2,   1, 4, 0b01);
    PEEK_BITS_TEST(stream_le, 3,   1, 7, 0b010);
    PEEK_BITS_TEST(stream_le, 4,   2, 3, 0b1010);
    PEEK_BITS_TEST(stream_le, 5,   3, 0, 0b10101);
    PEEK_BITS_TEST(stream_le, 6,   3, 6, 0b010101);
    PEEK_BITS_TEST(stream_le, 7,   4, 5, 0b0101010);
    PEEK_BITS_TEST(stream_le, 8,   5, 5, 0b10101010);
    PEEK_BITS_TEST(stream_le, 9,   6, 6, 0b101010101);
    PEEK_BITS_TEST(stream_le, 10,  8, 0, 0b0101010101);
    PEEK_BITS_TEST(stream_le, 11,  9, 3, 0b01010101010);
    PEEK_BITS_TEST(stream_le, 12, 10, 7, 0b101010101010);
    PEEK_BITS_TEST(stream_le, 13, 12, 4, 0b1010101010101);
    PEEK_BITS_TEST(stream_le, 14, 14, 2, 0b01010101010101);

    stream_le.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(14, 2), before);
            EXPECT_EQ(lts::istream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.peek_bits(15, value));

    std::cout << (std::string)stream_le << std::endl;
}

#define READ_EXP_GOLOMB_U_TEST(stream, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_exp_golomb_u(value));                                         \
    EXPECT_EQ(expected_bytepos, stream.tell());                                           \
    EXPECT_EQ(expected_bitpos, stream.tell_bits());                                       \
    EXPECT_EQ(expected_value, value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream, read_exp_golomb_u_be)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    READ_EXP_GOLOMB_U_TEST(stream_be, 0, 1, 0);
    READ_EXP_GOLOMB_U_TEST(stream_be, 0, 4, 1);
    READ_EXP_GOLOMB_U_TEST(stream_be, 0, 7, 2);
    READ_EXP_GOLOMB_U_TEST(stream_be, 1, 4, 3);
    READ_EXP_GOLOMB_U_TEST(stream_be, 2, 1, 4);
    READ_EXP_GOLOMB_U_TEST(stream_be, 2, 6, 5);
    READ_EXP_GOLOMB_U_TEST(stream_be, 3, 3, 6);
    READ_EXP_GOLOMB_U_TEST(stream_be, 4, 2, 7);
    READ_EXP_GOLOMB_U_TEST(stream_be, 5, 1, 8);
    READ_EXP_GOLOMB_U_TEST(stream_be, 6, 0, 9);

    stream_be.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(6, 0), before);
            EXPECT_EQ(lts::istream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_exp_golomb_u(value));

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, read_exp_golomb_u_le)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    READ_EXP_GOLOMB_U_TEST(stream_le, 0, 1, 0);
    READ_EXP_GOLOMB_U_TEST(stream_le, 0, 4, 1);
    READ_EXP_GOLOMB_U_TEST(stream_le, 0, 7, 2);
    READ_EXP_GOLOMB_U_TEST(stream_le, 1, 4, 3);
    READ_EXP_GOLOMB_U_TEST(stream_le, 2, 1, 4);
    READ_EXP_GOLOMB_U_TEST(stream_le, 2, 6, 5);
    READ_EXP_GOLOMB_U_TEST(stream_le, 3, 3, 6);
    READ_EXP_GOLOMB_U_TEST(stream_le, 4, 2, 7);
    READ_EXP_GOLOMB_U_TEST(stream_le, 5, 1, 8);
    READ_EXP_GOLOMB_U_TEST(stream_le, 6, 0, 9);

    stream_le.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(6, 0), before);
            EXPECT_EQ(lts::istream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_exp_golomb_u(value));

    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_EXP_GOLOMB_U_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_u(value));                \
    EXPECT_EQ(expected_value, value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream, peek_exp_golomb_u_be)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    PEEK_EXP_GOLOMB_U_TEST(stream_be, 1, 0);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 3, 1);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 3, 2);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 5, 3);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 5, 4);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 5, 5);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 5, 6);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 7, 7);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 7, 8);
    PEEK_EXP_GOLOMB_U_TEST(stream_be, 7, 9);

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, peek_exp_golomb_u_le)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    PEEK_EXP_GOLOMB_U_TEST(stream_le, 1, 0);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 3, 1);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 3, 2);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 5, 3);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 5, 4);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 5, 5);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 5, 6);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 7, 7);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 7, 8);
    PEEK_EXP_GOLOMB_U_TEST(stream_le, 7, 9);

    std::cout << (std::string)stream_le << std::endl;
}


#define READ_EXP_GOLOMB_S_TEST(stream, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_exp_golomb_s(value));                                         \
    EXPECT_EQ(expected_bytepos, stream.tell());                                           \
    EXPECT_EQ(expected_bitpos, stream.tell_bits());                                       \
    EXPECT_EQ(expected_value, value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream, read_exp_golomb_s_be)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    READ_EXP_GOLOMB_S_TEST(stream_be, 0, 1,  0);
    READ_EXP_GOLOMB_S_TEST(stream_be, 0, 4, +1);
    READ_EXP_GOLOMB_S_TEST(stream_be, 0, 7, -1);
    READ_EXP_GOLOMB_S_TEST(stream_be, 1, 4, +2);
    READ_EXP_GOLOMB_S_TEST(stream_be, 2, 1, -2);
    READ_EXP_GOLOMB_S_TEST(stream_be, 2, 6, +3);
    READ_EXP_GOLOMB_S_TEST(stream_be, 3, 3, -3);
    READ_EXP_GOLOMB_S_TEST(stream_be, 4, 2, +4);
    READ_EXP_GOLOMB_S_TEST(stream_be, 5, 1, -4);
    READ_EXP_GOLOMB_S_TEST(stream_be, 6, 0, +5);

    stream_be.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(6, 0), before);
            EXPECT_EQ(lts::istream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_exp_golomb_s(value));

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, read_exp_golomb_s_le)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    READ_EXP_GOLOMB_S_TEST(stream_le, 0, 1,  0);
    READ_EXP_GOLOMB_S_TEST(stream_le, 0, 4, +1);
    READ_EXP_GOLOMB_S_TEST(stream_le, 0, 7, -1);
    READ_EXP_GOLOMB_S_TEST(stream_le, 1, 4, +2);
    READ_EXP_GOLOMB_S_TEST(stream_le, 2, 1, -2);
    READ_EXP_GOLOMB_S_TEST(stream_le, 2, 6, +3);
    READ_EXP_GOLOMB_S_TEST(stream_le, 3, 3, -3);
    READ_EXP_GOLOMB_S_TEST(stream_le, 4, 2, +4);
    READ_EXP_GOLOMB_S_TEST(stream_le, 5, 1, -4);
    READ_EXP_GOLOMB_S_TEST(stream_le, 6, 0, +5);

    stream_le.set_error_function(
        [](const lts::istream_position& before, const lts::istream_position& after){
            EXPECT_EQ(lts::istream_position(6, 0), before);
            EXPECT_EQ(lts::istream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_exp_golomb_s(value));

    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_EXP_GOLOMB_S_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_s(value));                \
    EXPECT_EQ(expected_value, value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream, peek_exp_golomb_s_be)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_BIG_ENDIAN> stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    PEEK_EXP_GOLOMB_S_TEST(stream_be, 1,  0);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 3, +1);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 3, -1);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 5, +2);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 5, -2);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 5, +3);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 5, -3);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 7, +4);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 7, -4);
    PEEK_EXP_GOLOMB_S_TEST(stream_be, 7, +5);

    std::cout << (std::string)stream_be << std::endl;
}

TEST(istream, peek_exp_golomb_s_le)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream<STREAM_BYTE_ORDER_LITTLE_ENDIAN> stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    PEEK_EXP_GOLOMB_S_TEST(stream_le, 1,  0);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 3, +1);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 3, -1);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 5, +2);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 5, -2);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 5, +3);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 5, -3);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 7, +4);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 7, -4);
    PEEK_EXP_GOLOMB_S_TEST(stream_le, 7, +5);

    std::cout << (std::string)stream_le << std::endl;
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
