/* SPDX-License-Identifier: MIT */
/**
 * @file istream_be_test.cpp
 *
 * GoogleTests for istream_be class.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#include <gtest/gtest.h>
#include <memory>

#include "istream.hpp"

TEST(istream_be, create_onstack_nullptr)
{
    lts::istream_be stream_be(nullptr, 0);
    EXPECT_FALSE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_be.size(), 0);
    EXPECT_EQ(stream_be.data(), nullptr);
    EXPECT_EQ(stream_be.current_data_pointer(), nullptr);
    EXPECT_EQ(stream_be.tell(), 0);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    EXPECT_EQ(stream_be.remains(), 0);
}

TEST(istream_be, create_onstack)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_be stream_be(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_be.size(), std::size(buffer));
    EXPECT_EQ(stream_be.data(), buffer);
    EXPECT_EQ(stream_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream_be.tell(), 0);
    EXPECT_EQ(stream_be.tell_bits(), 0);
    EXPECT_EQ(stream_be.remains(), std::size(buffer));
}

TEST(istream_be, create_onheap_nullptr)
{
    std::unique_ptr<lts::istream_be> stream_be = std::make_unique<lts::istream_be>(nullptr, 0);
    EXPECT_FALSE(stream_be->is_valid());
    EXPECT_EQ(stream_be->status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_be->size(), 0);
    EXPECT_EQ(stream_be->data(), nullptr);
    EXPECT_EQ(stream_be->current_data_pointer(), nullptr);
    EXPECT_EQ(stream_be->tell(), 0);
    EXPECT_EQ(stream_be->tell_bits(), 0);
    EXPECT_EQ(stream_be->remains(), 0);
}

TEST(istream_be, create_onheap)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    std::unique_ptr<lts::istream_be> stream_be = std::make_unique<lts::istream_be>(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be->is_valid());
    EXPECT_EQ(stream_be->status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream_be->size(), std::size(buffer));
    EXPECT_EQ(stream_be->data(), buffer);
    EXPECT_EQ(stream_be->current_data_pointer(), buffer);
    EXPECT_EQ(stream_be->tell(), 0);
    EXPECT_EQ(stream_be->tell_bits(), 0);
    EXPECT_EQ(stream_be->remains(), std::size(buffer));
}

TEST(istream_be, create_copy_constructor)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_be stream1_be(buffer, std::size(buffer));
    lts::istream_be stream2_be(stream1_be);

    EXPECT_TRUE(stream2_be.is_valid());
    EXPECT_EQ(stream2_be.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream2_be.size(), std::size(buffer));
    EXPECT_EQ(stream2_be.data(), buffer);
    EXPECT_EQ(stream2_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_be.tell(), 0);
    EXPECT_EQ(stream2_be.tell_bits(), 0);
    EXPECT_EQ(stream2_be.remains(), std::size(buffer));
}

TEST(istream_be, create_copy_assigment)
{
    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};

    lts::istream_be stream1_be(buffer, std::size(buffer));
    lts::istream_be stream2_be;
    stream2_be = stream1_be;

    EXPECT_TRUE(stream2_be.is_valid());
    EXPECT_EQ(stream2_be.status(), ISTREAM_STATUS_OK);
    EXPECT_EQ(stream2_be.size(), std::size(buffer));
    EXPECT_EQ(stream2_be.data(), buffer);
    EXPECT_EQ(stream2_be.current_data_pointer(), buffer);
    EXPECT_EQ(stream2_be.tell(), 0);
    EXPECT_EQ(stream2_be.tell_bits(), 0);
    EXPECT_EQ(stream2_be.remains(), std::size(buffer));
}

TEST(istream_be, mark_corrupted)
{
    lts::istream_be stream_be(nullptr, 0);
    EXPECT_FALSE(stream_be.is_valid());
    EXPECT_EQ(stream_be.status(), ISTREAM_STATUS_OK);

    stream_be.mark_corrupted();
    EXPECT_EQ(stream_be.status(), ISTREAM_STATUS_STREAM_CORRUPTED);

    std::cout << (std::string)stream_be << std::endl;

    stream_be.clear_status();
    EXPECT_EQ(stream_be.status(), ISTREAM_STATUS_OK);
}

#include "read_peek_be_u8.test"
#include "read_peek_be_s8.test"

#include "read_peek_be_u16.test"
#include "read_peek_be_s16.test"

#include "read_peek_be_u32.test"
#include "read_peek_be_s32.test"

#include "read_peek_be_u64.test"
#include "read_peek_be_s64.test"

#define READ_BITS_TEST(stream, bits, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_bits(bits, value));                                         \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                         \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                     \
    EXPECT_EQ(value, expected_value);                                                   \
    std::cout << (std::string)stream << std::endl;

TEST(istream_be, read_bits)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_bits(15, value));

    stream_be.clear_error_function();
    std::cout << (std::string)stream_be << std::endl;
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

TEST(istream_be, peek_bits)
{
    uint32_t value;
    uint8_t buffer[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(14, 2), before);
            EXPECT_EQ(lts::stream_position(16, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.peek_bits(15, value));

    stream_be.clear_error_function();
    std::cout << (std::string)stream_be << std::endl;
}

#define READ_EXP_GOLOMB_U_TEST(stream, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_exp_golomb_u(value));                                         \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                           \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                       \
    EXPECT_EQ(value, expected_value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream_be, read_exp_golomb_u)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_exp_golomb_u(value));

    std::cout << (std::string)stream_be << std::endl;
}

#define PEEK_EXP_GOLOMB_U_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_u(value));                \
    EXPECT_EQ(value, expected_value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream_be, peek_exp_golomb_u)
{
    uint32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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

#define READ_EXP_GOLOMB_S_TEST(stream, expected_bytepos, expected_bitpos, expected_value) \
    EXPECT_TRUE(stream.read_exp_golomb_s(value));                                         \
    EXPECT_EQ(stream.tell(), expected_bytepos);                                           \
    EXPECT_EQ(stream.tell_bits(), expected_bitpos);                                       \
    EXPECT_EQ(value, expected_value);                                                     \
    std::cout << (std::string)stream << std::endl;

TEST(istream_be, read_exp_golomb_s)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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
        [](const lts::stream_position& before, const lts::stream_position& after){
            EXPECT_EQ(lts::stream_position(6, 0), before);
            EXPECT_EQ(lts::stream_position(6, 1), after);
        }
    );

    EXPECT_FALSE(stream_be.read_exp_golomb_s(value));

    std::cout << (std::string)stream_be << std::endl;
}

#define PEEK_EXP_GOLOMB_S_TEST(stream, skipbits, expected_value) \
    EXPECT_TRUE(stream.peek_exp_golomb_s(value));                \
    EXPECT_EQ(value, expected_value);                            \
    stream.skip_bits(skipbits);                                  \
    std::cout << (std::string)stream << std::endl;

TEST(istream_be, peek_exp_golomb_s)
{
    int32_t value;
    uint8_t buffer[] = {0xa6, 0x42, 0x98, 0xe2, 0x04, 0x8a};

    lts::istream_be stream_be(buffer, sizeof(buffer));
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

TEST(istream_be, read_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                    };

    lts::istream_be stream_be(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_TRUE(stream_be.read_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0102);
    EXPECT_EQ(std::get<2>(s1.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x08090a0b0c0d0e0full);

    EXPECT_TRUE(stream_be.read_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x10);
    EXPECT_EQ(std::get<1>(s2.members), 0x1112);
    EXPECT_EQ(std::get<2>(s2.members), 0x13141516);
    EXPECT_EQ(std::get<3>(s2.members), 0x17);
    EXPECT_EQ(std::get<4>(s2.members), 0x18191a1b1c1d1e1full);

    stream_be.rewind();

    EXPECT_TRUE(stream_be.read_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0102);
    EXPECT_EQ(std::get<2>(s1.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x08090a0b0c0d0e0full);

    EXPECT_TRUE(stream_be.read_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x10);
    EXPECT_EQ(std::get<1>(s2.members), 0x1112);
    EXPECT_EQ(std::get<2>(s2.members), 0x13141516);
    EXPECT_EQ(std::get<3>(s2.members), 0x17);
    EXPECT_EQ(std::get<4>(s2.members), 0x18191a1b1c1d1e1full);
}

TEST(istream_be, peak_structure)
{
    lts::structure<uint8_t, uint16_t, uint32_t, uint8_t, uint64_t> s1;
    lts::structure<int8_t, int16_t, int32_t, int8_t, int64_t> s2;

    uint8_t buffer[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                    };

    lts::istream_be stream_be(buffer, std::size(buffer));
    EXPECT_TRUE(stream_be.is_valid());

    EXPECT_TRUE(stream_be.peak_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0102);
    EXPECT_EQ(std::get<2>(s1.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x08090a0b0c0d0e0fULL);

    EXPECT_TRUE(stream_be.peak_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x00);
    EXPECT_EQ(std::get<1>(s2.members), 0x0102);
    EXPECT_EQ(std::get<2>(s2.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s2.members), 0x07);
    EXPECT_EQ(std::get<4>(s2.members), 0x08090a0b0c0d0e0fULL);

    stream_be.rewind();

    EXPECT_TRUE(stream_be.peak_structure(s1));
    EXPECT_EQ(std::get<0>(s1.members), 0x00);
    EXPECT_EQ(std::get<1>(s1.members), 0x0102);
    EXPECT_EQ(std::get<2>(s1.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s1.members), 0x07);
    EXPECT_EQ(std::get<4>(s1.members), 0x08090a0b0c0d0e0fULL);

    EXPECT_TRUE(stream_be.peak_structure(s2));
    EXPECT_EQ(std::get<0>(s2.members), 0x00);
    EXPECT_EQ(std::get<1>(s2.members), 0x0102);
    EXPECT_EQ(std::get<2>(s2.members), 0x03040506);
    EXPECT_EQ(std::get<3>(s2.members), 0x07);
    EXPECT_EQ(std::get<4>(s2.members), 0x08090a0b0c0d0e0fULL);
}
