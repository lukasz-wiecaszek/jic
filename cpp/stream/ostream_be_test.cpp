/* SPDX-License-Identifier: MIT */
/**
 * @file ostream_be_test.cpp
 *
 * GoogleTests for ostream_be class.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#include <gtest/gtest.h>
#include <memory>

#include "ostream.hpp"

TEST(ostream_be, create_onstack_nullptr)
{
    lts::ostream_be stream_be(nullptr, 0);
    EXPECT_FALSE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_be.size(), 0);
    EXPECT_EQ(stream_be.data(), nullptr);
    EXPECT_EQ(stream_be.current_data_pointer(), nullptr);
    EXPECT_EQ(stream_be.tell(), 0);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    EXPECT_EQ(stream_be.remains(), 0);
}

TEST(ostream_be, create_onstack)
{
    uint8_t buffer[16] = {0};

    lts::ostream_be stream_be(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_be.size(), std::size(buffer));
    EXPECT_EQ(stream_be.data(), buffer);
    EXPECT_EQ(stream_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream_be.tell(), 0);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    EXPECT_EQ(stream_be.remains(), std::size(buffer));
}

TEST(ostream_be, create_onheap_nullptr)
{
    std::unique_ptr<lts::ostream_be> stream_be = std::make_unique<lts::ostream_be>(nullptr, 0);
    EXPECT_FALSE(stream_be->is_valid());
    EXPECT_EQ(stream_be->status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_be->size(), 0);
    EXPECT_EQ(stream_be->data(), nullptr);
    EXPECT_EQ(stream_be->current_data_pointer(), nullptr);
    EXPECT_EQ(stream_be->tell(), 0);
    EXPECT_EQ(stream_be->tell_bits(), 0);
    EXPECT_EQ(stream_be->remains(), 0);
}

TEST(ostream_be, create_onheap)
{
    uint8_t buffer[16] = {0};

    std::unique_ptr<lts::ostream_be> stream_be = std::make_unique<lts::ostream_be>(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be->is_valid());
    EXPECT_EQ(stream_be->status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream_be->size(), std::size(buffer));
    EXPECT_EQ(stream_be->data(), buffer);
    EXPECT_EQ(stream_be->current_data_pointer(), buffer);
    EXPECT_EQ(stream_be->tell(), 0);
    EXPECT_EQ(stream_be->tell_bits(), 0);
    EXPECT_EQ(stream_be->remains(), std::size(buffer));
}

TEST(ostream_be, create_copy_constructor)
{
    uint8_t buffer[16] = {0};

    lts::ostream_be stream1_be(buffer, std::size(buffer));
    lts::ostream_be stream2_be(stream1_be);

    EXPECT_TRUE(stream2_be.is_valid());
    EXPECT_EQ(stream2_be.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream2_be.size(), std::size(buffer));
    EXPECT_EQ(stream2_be.data(), buffer);
    EXPECT_EQ(stream2_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_be.tell(), 0);
    EXPECT_EQ(stream2_be.tell_bits(), 0);
    EXPECT_EQ(stream2_be.remains(), std::size(buffer));
}

TEST(ostream_be, create_copy_assigment)
{
    uint8_t buffer[16] = {0};

    lts::ostream_be stream1_be(buffer, std::size(buffer));
    lts::ostream_be stream2_be;
    stream2_be = stream1_be;

    EXPECT_TRUE(stream2_be.is_valid());
    EXPECT_EQ(stream2_be.status(), OSTREAM_STATUS_OK);
    EXPECT_EQ(stream2_be.size(), std::size(buffer));
    EXPECT_EQ(stream2_be.data(), buffer);
    EXPECT_EQ(stream2_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_be.tell(), 0);
    EXPECT_EQ(stream2_be.tell_bits(), 0);
    EXPECT_EQ(stream2_be.remains(), std::size(buffer));
}

TEST(ostream_be, mark_corrupted)
{
    lts::ostream_be stream_be(nullptr, 0);
    EXPECT_FALSE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), OSTREAM_STATUS_OK);

    stream_be.mark_corrupted();
    EXPECT_EQ(stream_be.status(), OSTREAM_STATUS_STREAM_CORRUPTED);

    std::cout << (std::string)stream_be << std::endl;

    stream_be.clear_status();
    EXPECT_EQ(stream_be.status(), OSTREAM_STATUS_OK);
}

#include "write_be_u8.test"
#include "write_be_s8.test"

#include "write_be_u16.test"
#include "write_be_s16.test"

#include "write_be_u32.test"
#include "write_be_s32.test"

#include "write_be_u64.test"
#include "write_be_s64.test"

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

TEST(ostream_be, write_bits)
{
    uint8_t buffer[16] = {0};

    lts::ostream_be stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_EQ(0, stream_be.tell());
    EXPECT_EQ(0, stream_be.tell_bits());

    WRITE_BITS_TEST(stream_be, 1, 0, 0, 1, (std::initializer_list<uint8_t>({0b00000000})));
    WRITE_BITS_TEST(stream_be, 1, 1, 0, 2, (std::initializer_list<uint8_t>({0b01000000})));
    WRITE_BITS_TEST(stream_be, 1, 0, 0, 3, (std::initializer_list<uint8_t>({0b01000000})));
    WRITE_BITS_TEST(stream_be, 1, 1, 0, 4, (std::initializer_list<uint8_t>({0b01010000})));
    WRITE_BITS_TEST(stream_be, 1, 0, 0, 5, (std::initializer_list<uint8_t>({0b01010000})));
    WRITE_BITS_TEST(stream_be, 1, 1, 0, 6, (std::initializer_list<uint8_t>({0b01010100})));
    WRITE_BITS_TEST(stream_be, 1, 0, 0, 7, (std::initializer_list<uint8_t>({0b01010100})));
    WRITE_BITS_TEST(stream_be, 1, 1, 1, 0, (std::initializer_list<uint8_t>({0b01010101})));
    WRITE_BITS_TEST(stream_be, 1, 0, 1, 1, (std::initializer_list<uint8_t>({0b01010101, 0b00000000})));
    WRITE_BITS_TEST(stream_be, 1, 1, 1, 2, (std::initializer_list<uint8_t>({0b01010101, 0b01000000})));

    EXPECT_TRUE(stream_be.write_bits(2,  0b01));
    EXPECT_TRUE(stream_be.write_bits(3,  0b010));
    EXPECT_TRUE(stream_be.write_bits(4,  0b1010));
    EXPECT_TRUE(stream_be.write_bits(5,  0b10101));
    EXPECT_TRUE(stream_be.write_bits(6,  0b010101));
    EXPECT_TRUE(stream_be.write_bits(7,  0b0101010));
    EXPECT_TRUE(stream_be.write_bits(8,  0b10101010));
    EXPECT_TRUE(stream_be.write_bits(9,  0b101010101));
    EXPECT_TRUE(stream_be.write_bits(10, 0b0101010101));
    EXPECT_TRUE(stream_be.write_bits(11, 0b01010101010));
    EXPECT_TRUE(stream_be.write_bits(12, 0b101010101010));
    EXPECT_TRUE(stream_be.write_bits(13, 0b1010101010101));
    EXPECT_TRUE(stream_be.write_bits(14, 0b01010101010101));

    EXPECT_EQ(stream_be.tell(), 14);
    EXPECT_EQ(stream_be.tell_bits(), 2);
    ASSERT_NE(stream_be.data(), nullptr);
    const uint8_t* d = stream_be.data();
    uint8_t expected_buffer[15] = {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x40};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_be.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.write_bits(15, 0b010101010101010));

    stream_be.clear_error_function();
    std::cout << (std::string)stream_be << std::endl;
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

TEST(ostream_be, write_exp_golomb_u)
{
    uint8_t buffer[6] = {0};

    lts::ostream_be stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_EQ(0, stream_be.tell());
    EXPECT_EQ(0, stream_be.tell_bits());

    WRITE_EXP_GOLOMB_U_TEST(stream_be, 0, 0, 1, (std::initializer_list<uint8_t>({0b10000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 1, 0, 4, (std::initializer_list<uint8_t>({0b10100000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 2, 0, 7, (std::initializer_list<uint8_t>({0b10100110})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 3, 1, 4, (std::initializer_list<uint8_t>({0b10100110, 0b01000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 4, 2, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 5, 2, 6, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 6, 3, 3, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 7, 4, 2, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000000})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 8, 5, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100})));
    WRITE_EXP_GOLOMB_U_TEST(stream_be, 9, 6, 0, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100, 0b10001010})));

    EXPECT_EQ(stream_be.tell(), 6);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    ASSERT_NE(stream_be.data(), nullptr);
    const uint8_t* d = stream_be.data();
    uint8_t expected_buffer[6] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_be.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.write_exp_golomb_u(0));

    std::cout << (std::string)stream_be << std::endl;
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

TEST(ostream_be, write_exp_golomb_s)
{
    uint8_t buffer[6] = {0};

    lts::ostream_be stream_be(buffer, sizeof(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_EQ(0, stream_be.tell());
    EXPECT_EQ(0, stream_be.tell_bits());

    WRITE_EXP_GOLOMB_S_TEST(stream_be,  0, 0, 1, (std::initializer_list<uint8_t>({0b10000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, +1, 0, 4, (std::initializer_list<uint8_t>({0b10100000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, -1, 0, 7, (std::initializer_list<uint8_t>({0b10100110})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, +2, 1, 4, (std::initializer_list<uint8_t>({0b10100110, 0b01000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, -2, 2, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, +3, 2, 6, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, -3, 3, 3, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, +4, 4, 2, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000000})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, -4, 5, 1, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100})));
    WRITE_EXP_GOLOMB_S_TEST(stream_be, +5, 6, 0, (std::initializer_list<uint8_t>({0b10100110, 0b01000010, 0b10011000, 0b11100010, 0b00000100, 0b10001010})));

    EXPECT_EQ(stream_be.tell(), 6);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    ASSERT_NE(stream_be.data(), nullptr);
    const uint8_t* d = stream_be.data();
    uint8_t expected_buffer[6] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);

    stream_be.set_error_function(
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.write_exp_golomb_u(0));

    std::cout << (std::string)stream_be << std::endl;
}

TEST(ostream_be, write_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    std::get<0>(s1.members) = 0x00;
    std::get<1>(s1.members) = 0x0102;
    std::get<2>(s1.members) = 0x03040506;
    std::get<3>(s1.members) = 0x07;
    std::get<4>(s1.members) = 0x08090a0b0c0d0e0full;

    std::get<0>(s2.members) = 0x10;
    std::get<1>(s2.members) = 0x1112;
    std::get<2>(s2.members) = 0x13141516;
    std::get<3>(s2.members) = 0x17;
    std::get<4>(s2.members) = 0x18191a1b1c1d1e1full;

    uint8_t buffer[32] = {0};
    lts::ostream_be stream_be(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_TRUE(stream_be.write_structure(s1));
    EXPECT_TRUE(stream_be.write_structure(s2));

    EXPECT_EQ(stream_be.tell(), 32);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    ASSERT_NE(stream_be.data(), nullptr);
    const uint8_t* d = stream_be.data();
    uint8_t expected_buffer[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    };
    EXPECT_EQ(std::memcmp(d, expected_buffer, std::size(expected_buffer)), 0);
}
