/**
 * @file flatbuffer_test.cpp
 *
 * Test procedures for 'flatbuffer' type.
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
#include <iostream>
#include <climits>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "flatbuffer.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CHECK_AVAILABLE(fb, rd, wr)           \
    do {                                      \
        EXPECT_EQ(rd, fb.read_available());   \
        EXPECT_EQ(wr, fb.write_available());  \
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

TEST(flatbuffer, capacity)
{
    std::size_t capacity;

    {
        capacity = 42;
        lts::flatbuffer<int> fb(capacity);
        ASSERT_TRUE(fb.is_valid());
        EXPECT_EQ(capacity, fb.capacity());
    }

    {
        capacity = 0;
        lts::flatbuffer<int> fb(capacity);
        ASSERT_FALSE(fb.is_valid());
        EXPECT_EQ(capacity, fb.capacity());
    }
}

TEST(flatbuffer, initial_values)
{
    std::size_t capacity;

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_EQ(capacity, fb.write_available());
    EXPECT_EQ(0, fb.read_available());

    fb.reset();

    EXPECT_EQ(capacity, fb.write_available());
    EXPECT_EQ(0, fb.read_available());

    std::cout << (std::string)fb << std::endl;
}

TEST(flatbuffer, write_element)
{
    std::size_t capacity;
    int element = 0;

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_EQ(1, fb.write(0));
    CHECK_AVAILABLE(fb, 1, 41);

    EXPECT_EQ(1, fb.write(element));
    CHECK_AVAILABLE(fb, 2, 40);

    EXPECT_EQ(1,  fb.write(0 + element));
    CHECK_AVAILABLE(fb, 3, 39);

    std::cout << (std::string)fb << std::endl;
}

TEST(flatbuffer, write_array)
{
    std::size_t capacity;
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_EQ(10, fb.write(array));
    CHECK_AVAILABLE(fb, 10, 32);

    EXPECT_EQ(10, fb.write(array));
    CHECK_AVAILABLE(fb, 20, 22);

    EXPECT_EQ(10, fb.write(array));
    CHECK_AVAILABLE(fb, 30, 12);

    EXPECT_EQ(10, fb.write(array));
    CHECK_AVAILABLE(fb, 40, 2);

    EXPECT_EQ(2,  fb.write(array));
    CHECK_AVAILABLE(fb, 42, 0);

    EXPECT_EQ(0,  fb.write(array));
    CHECK_AVAILABLE(fb, 42, 0);

    std::cout << (std::string)fb << std::endl;
}

TEST(flatbuffer, write_and_consume)
{
    std::size_t capacity;
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    {
        EXPECT_EQ(10, fb.write(array));
        CHECK_AVAILABLE(fb, 10, 32);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 7, 32);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 4, 32);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 1, 32);

        EXPECT_EQ(1,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 32);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 32);

        std::cout << (std::string)fb << std::endl;
    }

    {
        EXPECT_EQ(10, fb.write(array));
        CHECK_AVAILABLE(fb, 10, 22);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 7, 22);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 4, 22);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 1, 22);

        EXPECT_EQ(1,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 22);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 22);

        std::cout << (std::string)fb << std::endl;
    }

    {
        EXPECT_EQ(10, fb.write(array));
        CHECK_AVAILABLE(fb, 10, 12);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 7, 12);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 4, 12);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 1, 12);

        EXPECT_EQ(1,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 12);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 12);

        std::cout << (std::string)fb << std::endl;
    }

    {
        EXPECT_EQ(10, fb.write(array));
        CHECK_AVAILABLE(fb, 10, 2);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 7, 2);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 4, 2);

        EXPECT_EQ(3,  fb.consume(3));
        CHECK_AVAILABLE(fb, 1, 2);

        EXPECT_EQ(1,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 2);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 2);

        std::cout << (std::string)fb << std::endl;
    }

    {
        EXPECT_EQ(2,  fb.write(array));
        CHECK_AVAILABLE(fb, 2, 0);

        EXPECT_EQ(2,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        std::cout << (std::string)fb << std::endl;
    }

    {
        EXPECT_EQ(0,  fb.write(array));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        EXPECT_EQ(0,  fb.consume(3));
        CHECK_AVAILABLE(fb, 0, 0);

        std::cout << (std::string)fb << std::endl;
    }
}

TEST(flatbuffer, read_ptr)
{
    std::size_t capacity;
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const int* p;

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_TRUE(nullptr != (p = fb.read_ptr()));

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(p, fb.read_ptr());

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(p, fb.read_ptr());

    EXPECT_EQ(0, fb.consume(0));
    EXPECT_EQ(p, fb.read_ptr());

    EXPECT_EQ(1, fb.consume(1));
    EXPECT_EQ(p += 1, fb.read_ptr());

    EXPECT_EQ(2, fb.consume(2));
    EXPECT_EQ(p += 2, fb.read_ptr());

    EXPECT_EQ(3, fb.consume(3));
    EXPECT_EQ(p += 3, fb.read_ptr());

    EXPECT_EQ(4, fb.consume(4));
    EXPECT_EQ(p += 4, fb.read_ptr());

    /* we have written 20 elements to the flatbuffer, already consumed 10,
    so we have only 10 more left to be consumed */
    EXPECT_EQ(10, fb.consume(11));
    EXPECT_EQ(p += 10, fb.read_ptr());

    std::cout << (std::string)fb << std::endl;
}

TEST(flatbuffer, bookmark)
{
    std::size_t capacity;
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const int* p;
    const int* b;

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_TRUE(nullptr != (p = fb.read_ptr()));
    EXPECT_TRUE(nullptr == (b = fb.get_bookmark()));

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(p, fb.read_ptr());
    EXPECT_EQ(nullptr, fb.get_bookmark());

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(p, fb.read_ptr());
    EXPECT_EQ(nullptr, (b = fb.get_bookmark()));

    fb.set_bookmark();
    EXPECT_EQ(p, (b = fb.get_bookmark()));

    fb.clear_bookmark();
    EXPECT_EQ(nullptr, (b = fb.get_bookmark()));

    fb.set_bookmark();
    EXPECT_EQ(p, (b = fb.get_bookmark()));

    EXPECT_EQ(0, fb.consume(0));
    EXPECT_EQ(p, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    EXPECT_EQ(1, fb.consume(1));
    EXPECT_EQ(p += 1, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    fb.set_bookmark();
    EXPECT_EQ(p, (b = fb.get_bookmark()));

    EXPECT_EQ(2, fb.consume(2));
    EXPECT_EQ(p += 2, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    EXPECT_EQ(3, fb.consume(3));
    EXPECT_EQ(p += 3, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    EXPECT_EQ(4, fb.consume(4));
    EXPECT_EQ(p += 4, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    /* we have written 20 elements to the flatbuffer, already consumed 10,
    so we have only 10 more left to be consumed */
    EXPECT_EQ(10, fb.consume(11));
    EXPECT_EQ(p += 10, fb.read_ptr());
    EXPECT_EQ(b, fb.get_bookmark());

    std::cout << (std::string)fb << std::endl;
}

TEST(flatbuffer, move)
{
    std::size_t capacity;
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const int* p;
    const int* b;

    capacity = 42;
    lts::flatbuffer<int> fb(capacity);
    ASSERT_TRUE(fb.is_valid());

    EXPECT_TRUE(nullptr != (p = fb.read_ptr()));
    EXPECT_TRUE(nullptr == (b = fb.get_bookmark()));

    EXPECT_EQ(p, fb.read_ptr());
    EXPECT_EQ(nullptr, fb.get_bookmark());
    CHECK_AVAILABLE(fb, 0, 42);

    fb.move();
    CHECK_AVAILABLE(fb, 0, 42);

    EXPECT_EQ(p, fb.read_ptr());
    EXPECT_EQ(nullptr, fb.get_bookmark());
    CHECK_AVAILABLE(fb, 0, 42);

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(10, fb.consume(10));
    CHECK_AVAILABLE(fb, 0, 32);
    fb.move();
    CHECK_AVAILABLE(fb, 0, 42);

    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(10, fb.write(array));
    EXPECT_EQ(10, fb.consume(10));
    fb.set_bookmark();
    EXPECT_EQ(10, fb.consume(10));
    CHECK_AVAILABLE(fb, 0, 22);
    fb.move();
    CHECK_AVAILABLE(fb, 0, 32);
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
