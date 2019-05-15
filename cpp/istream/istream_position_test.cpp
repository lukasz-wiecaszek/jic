/**
 * @file istream_position_test.cpp
 *
 * Test procedures for 'istream_position' type.
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
#include "istream_position.hpp"

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

TEST(istream_position, create_1_onstack)
{
    ymn::istream_position stream_position = ymn::istream_position();

    EXPECT_EQ(0, stream_position.m_byte_position);
    EXPECT_EQ(0, stream_position.m_bit_position);

    std::cout << (std::string)stream_position << std::endl;
}

TEST(istream_position, create_1_onheap)
{
    ymn::istream_position* stream_position = new ymn::istream_position();

    EXPECT_EQ(0, stream_position->m_byte_position);
    EXPECT_EQ(0, stream_position->m_bit_position);

    std::cout << (std::string)*stream_position << std::endl;

    delete stream_position; /* that will reach the second branch of the destructor */
}

TEST(istream_position, create_2)
{
    ymn::istream_position stream_position = ymn::istream_position(42);

    EXPECT_EQ(42, stream_position.m_byte_position);
    EXPECT_EQ(0, stream_position.m_bit_position);

    std::cout << (std::string)stream_position << std::endl;
}

TEST(istream_position, create_3)
{
    ymn::istream_position stream_position = ymn::istream_position(42, 24);

    EXPECT_EQ(42, stream_position.m_byte_position);
    EXPECT_EQ(24, stream_position.m_bit_position);

    std::cout << (std::string)stream_position << std::endl;
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
