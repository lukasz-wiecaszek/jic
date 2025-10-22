/* SPDX-License-Identifier: MIT */
/**
 * @file istream_le_test.cpp
 *
 * GoogleTests for istream_le class.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#include <gtest/gtest.h>
#include <memory>

#include "istream.hpp"

TEST(istream_le, create_onstack_nullptr)
{
    lts::istream_le stream_le(nullptr, 0);
    EXPECT_FALSE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_le.size(), 0);
    EXPECT_EQ(stream_le.data(), nullptr);
    EXPECT_EQ(stream_le.current_data_pointer(), nullptr);
    EXPECT_EQ(stream_le.tell(), 0);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    EXPECT_EQ(stream_le.remains(), 0);
}

TEST(istream_le, create_onstack)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_le stream_le(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_le.size(), std::size(buffer));
    EXPECT_EQ(stream_le.data(), buffer);
    EXPECT_EQ(stream_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream_le.tell(), 0);
    EXPECT_EQ(stream_le.tell_bits(), 0);
    EXPECT_EQ(stream_le.remains(), std::size(buffer));
}

TEST(istream_le, create_onheap_nullptr)
{
    std::unique_ptr<lts::istream_le> stream_le = std::make_unique<lts::istream_le>(nullptr, 0);
    EXPECT_FALSE(stream_le->is_valid());
    EXPECT_EQ(stream_le->status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_le->size(), 0);
    EXPECT_EQ(stream_le->data(), nullptr);
    EXPECT_EQ(stream_le->current_data_pointer(), nullptr);
    EXPECT_EQ(stream_le->tell(), 0);
    EXPECT_EQ(stream_le->tell_bits(), 0);
    EXPECT_EQ(stream_le->remains(), 0);
}

TEST(istream_le, create_onheap)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    std::unique_ptr<lts::istream_le> stream_le = std::make_unique<lts::istream_le>(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le->is_valid());
    EXPECT_EQ(stream_le->status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_le->size(), std::size(buffer));
    EXPECT_EQ(stream_le->data(), buffer);
    EXPECT_EQ(stream_le->current_data_pointer(), buffer);
    EXPECT_EQ(stream_le->tell(), 0);
    EXPECT_EQ(stream_le->tell_bits(), 0);
    EXPECT_EQ(stream_le->remains(), std::size(buffer));
}

TEST(istream_le, create_copy_constructor)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_le stream1_le(buffer, std::size(buffer));
    lts::istream_le stream2_le(stream1_le);

    EXPECT_TRUE(stream2_le.is_valid());
    EXPECT_EQ(stream2_le.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream2_le.size(), std::size(buffer));
    EXPECT_EQ(stream2_le.data(), buffer);
    EXPECT_EQ(stream2_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_le.tell(), 0);
    EXPECT_EQ(stream2_le.tell_bits(), 0);
    EXPECT_EQ(stream2_le.remains(), std::size(buffer));
}

TEST(istream_le, create_copy_assigment)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_le stream1_le(buffer, std::size(buffer));
    lts::istream_le stream2_le;
    stream2_le = stream1_le;

    EXPECT_TRUE(stream2_le.is_valid());
    EXPECT_EQ(stream2_le.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream2_le.size(), std::size(buffer));
    EXPECT_EQ(stream2_le.data(), buffer);
    EXPECT_EQ(stream2_le.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_le.tell(), 0);
    EXPECT_EQ(stream2_le.tell_bits(), 0);
    EXPECT_EQ(stream2_le.remains(), std::size(buffer));
}

TEST(istream_le, mark_corrupted)
{
    lts::istream_le stream_le(nullptr, 0);
    EXPECT_FALSE(stream_le.is_valid());
    EXPECT_EQ(stream_le.status(), ISTREAM_STATUS_OK);

    stream_le.mark_corrupted();
    EXPECT_EQ(stream_le.status(), ISTREAM_STATUS_STREAM_CORRUPTED);

    std::cout << (std::string)stream_le << std::endl;

    stream_le.clear_status();
    EXPECT_EQ(stream_le.status(), ISTREAM_STATUS_OK);
}

#include "read_peek_le_u8.test"
#include "read_peek_le_s8.test"

#include "read_peek_le_u16.test"
#include "read_peek_le_s16.test"

#include "read_peek_le_u32.test"
#include "read_peek_le_s32.test"

#include "read_peek_le_u64.test"
#include "read_peek_le_s64.test"

#define READ_BITS_TEST(stream, bits, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_bits(bits, value));                                         \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                         \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                     \
    EXPECT_EQ(value, expected_value);                                                   \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, read_bits)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_bits(15, value));

    stream_le.clear_error_function();
    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_BITS_TEST(stream, bits, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    EXPECT_TRUE(stream.peek_bits(bits, value));                                         \
    stream.skip_bits(bits);                                                             \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                         \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                     \
    EXPECT_EQ(value, expected_value);                                                   \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, peek_bits)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.peek_bits(15, value));

    stream_le.clear_error_function();
    std::cout << (std::string)stream_le << std::endl;
}

#define READ_EXP_GOLOMB_U_TEST(stream, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_exp_golomb_u(value));                                         \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                           \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                       \
    EXPECT_EQ(value, expected_value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, read_exp_golomb_u)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_exp_golomb_u(value));

    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_EXP_GOLOMB_U_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_u(value));                \
    EXPECT_EQ(value, expected_value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, peek_exp_golomb_u)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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
    EXPECT_EQ(stream.tell(), expected_bytepos);                                           \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                       \
    EXPECT_EQ(value, expected_value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, read_exp_golomb_s)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_le.read_exp_golomb_s(value));

    std::cout << (std::string)stream_le << std::endl;
}

#define PEEK_EXP_GOLOMB_S_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_s(value));                \
    EXPECT_EQ(value, expected_value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream_le, peek_exp_golomb_s)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_le stream_le(buffer, sizeof(buffer));
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

TEST(istream_le, read_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                    };

    lts::istream_le stream_le(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_TRUE(stream_le.read_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0201);
    EXPECT_EQ(std::get<2>(s1.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x0f0e0d0c0b0a0908ULL);

    EXPECT_TRUE(stream_le.read_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x10);
    EXPECT_EQ(std::get<1>(s2.members), 0x1211);
    EXPECT_EQ(std::get<2>(s2.members), 0x16151413);
    EXPECT_EQ(std::get<3>(s2.members), 0x17);
    EXPECT_EQ(std::get<4>(s2.members), 0x1f1e1d1c1b1a1918ULL);

    stream_le.rewind();

    EXPECT_TRUE(stream_le.read_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0201);
    EXPECT_EQ(std::get<2>(s1.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x0f0e0d0c0b0a0908ULL);

    EXPECT_TRUE(stream_le.read_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x10);
    EXPECT_EQ(std::get<1>(s2.members), 0x1211);
    EXPECT_EQ(std::get<2>(s2.members), 0x16151413);
    EXPECT_EQ(std::get<3>(s2.members), 0x17);
    EXPECT_EQ(std::get<4>(s2.members), 0x1f1e1d1c1b1a1918ULL);
}

TEST(istream_le, peak_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                    };

    lts::istream_le stream_le(buffer, std::size(buffer));
    EXPECT_TRUE(stream_le.is_valid());

    EXPECT_TRUE(stream_le.peak_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0201);
    EXPECT_EQ(std::get<2>(s1.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x0f0e0d0c0b0a0908ULL);

    EXPECT_TRUE(stream_le.peak_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x00);
    EXPECT_EQ(std::get<1>(s2.members), 0x0201);
    EXPECT_EQ(std::get<2>(s2.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s2.members), 0x07);
    EXPECT_EQ(std::get<4>(s2.members), 0x0f0e0d0c0b0a0908ULL);

    stream_le.rewind();

    EXPECT_TRUE(stream_le.peak_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0201);
    EXPECT_EQ(std::get<2>(s1.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x0f0e0d0c0b0a0908ULL);

    EXPECT_TRUE(stream_le.peak_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x00);
    EXPECT_EQ(std::get<1>(s2.members), 0x0201);
    EXPECT_EQ(std::get<2>(s2.members), 0x06050403);
    EXPECT_EQ(std::get<3>(s2.members), 0x07);
    EXPECT_EQ(std::get<4>(s2.members), 0x0f0e0d0c0b0a0908ULL);
}
