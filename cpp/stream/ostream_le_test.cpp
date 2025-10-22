/* SPDX-License-Identifier: MIT */
/**
 * @file ostream_le_test.cpp
 *
 * GoogleTests for ostream_le class.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#include <gtest/gtest.h>
#include <memory>

#include "ostream.hpp"

TEST(ostream_le, create_onstack_nullptr)
{
    lts::ostream_le stream_le(nullptr, 0);
    EXPECT_FALSE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_le.size(), 0);
    EXPECT_EQ(stream_le.data(), nullptr);
    EXPECT_EQ(stream_le.current_data_pointer(), nullptr);
    EXPECT_EQ(stream_le.tell(), 0);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    EXPECT_EQ(stream_le.remains(), 0);
}

TEST(ostream_le, create_onstack)
{
    uint8_t buffer[16] = {0};

    lts::ostream_le stream_le(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_le.size(), std::size(buffer));
    EXPECT_EQ(stream_le.data(), buffer);
    EXPECT_EQ(stream_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream_le.tell(), 0);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    EXPECT_EQ(stream_le.remains(), std::size(buffer));
}

TEST(ostream_le, create_onheap_nullptr)
{
    std::unique_ptr<lts::ostream_le> stream_le = std::make_unique<lts::ostream_le>(nullptr, 0);
    EXPECT_FALSE(stream_le->is_valid());
    EXPECT_EQ(stream_le->status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_le->size(), 0);
    EXPECT_EQ(stream_le->data(), nullptr);
    EXPECT_EQ(stream_le->current_data_pointer(), nullptr);
    EXPECT_EQ(stream_le->tell(), 0);
    EXPECT_EQ(stream_le->tell_bits(), 0);
    EXPECT_EQ(stream_le->remains(), 0);
}

TEST(ostream_le, create_onheap)
{
    uint8_t buffer[16] = {0};

    std::unique_ptr<lts::ostream_le> stream_le = std::make_unique<lts::ostream_le>(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le->is_valid());
    EXPECT_EQ(stream_le->status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_le->size(), std::size(buffer));
    EXPECT_EQ(stream_le->data(), buffer);
    EXPECT_EQ(stream_le->current_data_pointer(), buffer);
    EXPECT_EQ(stream_le->tell(), 0);
    EXPECT_EQ(stream_le->tell_bits(), 0);
    EXPECT_EQ(stream_le->remains(), std::size(buffer));
}

TEST(ostream_le, create_copy_constructor)
{
    uint8_t buffer[16] = {0};

    lts::ostream_le stream1_le(buffer, std::size(buffer));
    lts::ostream_le stream2_le(stream1_le);

    EXPECT_TRUE(stream2_le.is_valid());
    EXPECT_EQ(stream2_le.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream2_le.size(), std::size(buffer));
    EXPECT_EQ(stream2_le.data(), buffer);
    EXPECT_EQ(stream2_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_le.tell(), 0);
    EXPECT_EQ(stream2_le.tell_bits(), 0);
    EXPECT_EQ(stream2_le.remains(), std::size(buffer));
}

TEST(ostream_le, create_copy_assigment)
{
    uint8_t buffer[16] = {0};

    lts::ostream_le stream1_le(buffer, std::size(buffer));
    lts::ostream_le stream2_le;
    stream2_le = stream1_le;

    EXPECT_TRUE(stream2_le.is_valid());
    EXPECT_EQ(stream2_le.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream2_le.size(), std::size(buffer));
    EXPECT_EQ(stream2_le.data(), buffer);
    EXPECT_EQ(stream2_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_le.tell(), 0);
    EXPECT_EQ(stream2_le.tell_bits(), 0);
    EXPECT_EQ(stream2_le.remains(), std::size(buffer));
}

TEST(ostream_le, mark_corrupted)
{
    lts::ostream_le stream_le(nullptr, 0);
    EXPECT_FALSE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), OSTREAM_STATUS_OK);

    stream_le.mark_corrupted();
    EXPECT_EQ(stream_le.status(), OSTREAM_STATUS_STREAM_CORRUPTED);

    std::cout << (std::string)stream_le << std::endl;

    stream_le.clear_status();
    EXPECT_EQ(stream_le.status(), OSTREAM_STATUS_OK);
}

#include "write_le_u8.test"
#include "write_le_s8.test"

#include "write_le_u16.test"
#include "write_le_s16.test"

#include "write_le_u32.test"
#include "write_le_s32.test"

#include "write_le_u64.test"
#include "write_le_s64.test"

#define WRITE_BITS_TEST(stream, bits, value, expected_bytepos, expected_bitpos, expected_bitset) \
    EXPECT_TRUE(stream.write_bits(bits, value));                                                 \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                                  \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                              \
    ASSERT_NE(stream.data(), nullptr);                                                           \
    {                                                                                            \
        const uint8_t* d = stream.data();                                                        \
        for (std::size_t i = 0; i < expected_bitset.size(); i++)                                 \
            EXPECT_EQ(d[i], expected_bitset.begin()[i]);                                         \
    }                                                                                            \
    std::cout << (std::string)stream << std::endl;

TEST(ostream_le, write_bits)
{
    uint8_t buffer[16] = {0};

    lts::ostream_le stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_EQ(0, stream_le.tell());
    EXPECT_EQ(0, stream_le.tell_bits());

    WRITE_BITS_TEST(stream_le, 1, 0, 0, 1, (std::initializer_list<uint8_t>({0b00000000})));
    WRITE_BITS_TEST(stream_le, 1, 1, 0, 2, (std::initializer_list<uint8_t>({0b01000000})));
    WRITE_BITS_TEST(stream_le, 1, 0, 0, 3, (std::initializer_list<uint8_t>({0b01000000})));
    WRITE_BITS_TEST(stream_le, 1, 1, 0, 4, (std::initializer_list<uint8_t>({0b01010000})));
    WRITE_BITS_TEST(stream_le, 1, 0, 0, 5, (std::initializer_list<uint8_t>({0b01010000})));
    WRITE_BITS_TEST(stream_le, 1, 1, 0, 6, (std::initializer_list<uint8_t>({0b01010100})));
    WRITE_BITS_TEST(stream_le, 1, 0, 0, 7, (std::initializer_list<uint8_t>({0b01010100})));
    WRITE_BITS_TEST(stream_le, 1, 1, 1, 0, (std::initializer_list<uint8_t>({0b01010101})));
    WRITE_BITS_TEST(stream_le, 1, 0, 1, 1, (std::initializer_list<uint8_t>({0b01010101, 0b00000000})));
    WRITE_BITS_TEST(stream_le, 1, 1, 1, 2, (std::initializer_list<uint8_t>({0b01010101, 0b01000000})));

    EXPECT_TRUE(stream_le.write_bits(2,  0b01));
    EXPECT_TRUE(stream_le.write_bits(3,  0b010));
    EXPECT_TRUE(stream_le.write_bits(4,  0b1010));
    EXPECT_TRUE(stream_le.write_bits(5,  0b10101));
    EXPECT_TRUE(stream_le.write_bits(6,  0b010101));
    EXPECT_TRUE(stream_le.write_bits(7,  0b0101010));
    EXPECT_TRUE(stream_le.write_bits(8,  0b10101010));
    EXPECT_TRUE(stream_le.write_bits(9,  0b101010101));
    EXPECT_TRUE(stream_le.write_bits(10, 0b0101010101));
    EXPECT_TRUE(stream_le.write_bits(11, 0b01010101010));
    EXPECT_TRUE(stream_le.write_bits(12, 0b101010101010));
    EXPECT_TRUE(stream_le.write_bits(13, 0b1010101010101));
    EXPECT_TRUE(stream_le.write_bits(14, 0b01010101010101));

    EXPECT_EQ(stream_le.tell(), 14);
    EXPECT_EQ(stream_le.tell_bits(), 2);
    ASSERT_NE(stream_le.data(), nullptr);
    const uint8_t* d = stream_le.data();
    uint8_t expected_buffer[15] = {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x40};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_le.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.write_bits(15, 0b010101010101010));

    stream_le.clear_error_function();
    std::cout << (std::string)stream_le << std::endl;
}

#define WRITE_EXP_GOLOMB_U_TEST(stream, value, expected_bytepos, expected_bitpos, expected_bitset) \
    EXPECT_TRUE(stream.write_exp_golomb_u(value));                                                 \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                                    \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                                \
    ASSERT_NE(stream.data(), nullptr);                                                             \
    {                                                                                              \
        const uint8_t* d = stream.data();                                                          \
        for (std::size_t i = 0; i < expected_bitset.size(); i++)                                   \
            EXPECT_EQ(d[i], expected_bitset.begin()[i]);                                           \
    }                                                                                              \
    std::cout << (std::string)stream << std::endl;

TEST(ostream_le, write_exp_golomb_u)
{
    uint8_t buffer[6] = {0};

    lts::ostream_le stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_EQ(0, stream_le.tell());
    EXPECT_EQ(0, stream_le.tell_bits());

    WRITE_EXP_GOLOMB_U_TEST(stream_le, 0, 0, 1, (std::initializer_list<uint8_t>({0b10000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 1, 0, 4, (std::initializer_list<uint8_t>({0b10100000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 2, 0, 7, (std::initializer_list<uint8_t>({0b10100110})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 3, 1, 4, (std::initializer_list<uint8_t>({0b10100110, 0b01000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 4, 2, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 5, 2, 6, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 6, 3, 3, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 7, 4, 2, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 8, 5, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100})));
    WRITE_EXP_GOLOMB_U_TEST(stream_le, 9, 6, 0, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100, 0b10001010})));

    EXPECT_EQ(stream_le.tell(), 6);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    ASSERT_NE(stream_le.data(), nullptr);
    const uint8_t* d = stream_le.data();
    uint8_t expected_buffer[6] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_le.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.write_exp_golomb_u(0));

    std::cout << (std::string)stream_le << std::endl;
}

#define WRITE_EXP_GOLOMB_S_TEST(stream, value, expected_bytepos, expected_bitpos, expected_bitset) \
    EXPECT_TRUE(stream.write_exp_golomb_s(value));                                                 \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                                    \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                                \
    ASSERT_NE(stream.data(), nullptr);                                                             \
    {                                                                                              \
        const uint8_t* d = stream.data();                                                          \
        for (std::size_t i = 0; i < expected_bitset.size(); i++)                                   \
            EXPECT_EQ(d[i], expected_bitset.begin()[i]);                                           \
    }                                                                                              \
    std::cout << (std::string)stream << std::endl;

TEST(ostream_le, write_exp_golomb_s)
{
    uint8_t buffer[6] = {0};

    lts::ostream_le stream_le(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_EQ(0, stream_le.tell());
    EXPECT_EQ(0, stream_le.tell_bits());

    WRITE_EXP_GOLOMB_S_TEST(stream_le,  0, 0, 1, (std::initializer_list<uint8_t>({0b10000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, +1, 0, 4, (std::initializer_list<uint8_t>({0b10100000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, -1, 0, 7, (std::initializer_list<uint8_t>({0b10100110})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, +2, 1, 4, (std::initializer_list<uint8_t>({0b10100110, 0b01000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, -2, 2, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, +3, 2, 6, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, -3, 3, 3, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, +4, 4, 2, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, -4, 5, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100})));
    WRITE_EXP_GOLOMB_S_TEST(stream_le, +5, 6, 0, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100, 0b10001010})));

    EXPECT_EQ(stream_le.tell(), 6);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    ASSERT_NE(stream_le.data(), nullptr);
    const uint8_t* d = stream_le.data();
    uint8_t expected_buffer[6] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_le.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.write_exp_golomb_u(0));

    std::cout << (std::string)stream_le << std::endl;
}

TEST(ostream_le, write_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    std::get<0>(s1.members) = 0x00;
    std::get<1>(s1.members) = 0x0201;
    std::get<2>(s1.members) = 0x06050403;
    std::get<3>(s1.members) = 0x07;
    std::get<4>(s1.members) = 0x0f0e0d0c0b0a0908ull;

    std::get<0>(s2.members) = 0x10;
    std::get<1>(s2.members) = 0x1211;
    std::get<2>(s2.members) = 0x16151413;
    std::get<3>(s2.members) = 0x17;
    std::get<4>(s2.members) = 0x1f1e1d1c1b1a1918ull;

    uint8_t buffer[32] = {0};
    lts::ostream_le stream_le(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_TRUE(stream_le.write_structure(s1));
    EXPECT_TRUE(stream_le.write_structure(s2));

    EXPECT_EQ(stream_le.tell(), 32);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    ASSERT_NE(stream_le.data(), nullptr);
    const uint8_t* d = stream_le.data();
    uint8_t expected_buffer[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    };
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);
}
