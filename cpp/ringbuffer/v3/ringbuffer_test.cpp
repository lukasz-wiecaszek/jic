/**
 * @file ringbuffer_test.cpp
 *
 * Test procedures for 'ringbuffer' implementation.
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
#include <thread>
#include <mutex>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "ringbuffer.hpp"

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

#define ITERATIONS 1000

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
template<typename RB> static void producer_1element(RB& rb);
template<typename RB> static void consumer_1element(RB& rb);
template<typename RB, std::size_t N> static void producer_Nelements(RB& rb);
template<typename RB, std::size_t N> static void consumer_Nelements(RB& rb);

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/
static std::mutex mutex;

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
namespace
{

TEST(ringbuffer, create_on_stack)
{
    const std::size_t capacity = 1;
    ymn::ringbuffer<std::size_t> rb1(capacity, true);
    ymn::ringbuffer<std::size_t> rb2(capacity, false);

    EXPECT_EQ(capacity, rb1.capacity());
    EXPECT_EQ(capacity, rb2.capacity());

    EXPECT_EQ(true, rb1.non_blocking());
    EXPECT_EQ(false, rb2.non_blocking());

    std::cout << static_cast<std::string>(rb1) << std::endl;
    std::cout << static_cast<std::string>(rb2) << std::endl;
}

TEST(ringbuffer, create_on_heap)
{
    const std::size_t capacity = 1;

    ymn::ringbuffer<std::size_t> *rb1 =
        new ymn::ringbuffer<std::size_t>(capacity, true);
    ymn::ringbuffer<std::size_t> *rb2 =
        new ymn::ringbuffer<std::size_t>(capacity, false);

    EXPECT_EQ(capacity, rb1->capacity());
    EXPECT_EQ(capacity, rb2->capacity());

    EXPECT_EQ(true, rb1->non_blocking());
    EXPECT_EQ(false, rb2->non_blocking());

    std::cout << rb1->to_string() << std::endl;
    std::cout << rb2->to_string() << std::endl;

    delete rb1;
    delete rb2;
}

/*===========================================================================*\
 * tests of blocking semantic of ringbuffer
\*===========================================================================*/
TEST(ringbuffer, capacity1_1in_1out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(1, false);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity2_1in_1out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(2, false);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_1in_1out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(64, false);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_1in_1out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(65, false);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_1in_1out_as_array_blocking)
{
    ymn::ringbuffer<std::size_t> rb(64, false);

    std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_1in_1out_as_array_blocking)
{
    ymn::ringbuffer<std::size_t> rb(65, false);

    std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_7in_11out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(64, false);

    std::thread producer {producer_Nelements<decltype(rb), 7>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_7in_11out_blocking)
{
    ymn::ringbuffer<std::size_t> rb(65, false);

    std::thread producer {producer_Nelements<decltype(rb), 7>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

/*===========================================================================*\
 * tests of non-blocking semantic of ringbuffer
\*===========================================================================*/
TEST(ringbuffer, capacity1_1in_1out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(1, true);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity2_1in_1out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(2, true);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_1in_1out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(64, true);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_1in_1out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(65, true);

    std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};
    std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_1in_1out_as_array_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(64, true);

    std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_1in_1out_as_array_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(65, true);

    std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity64_7in_11out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(64, true);

    std::thread producer {producer_Nelements<decltype(rb), 7>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
}

TEST(ringbuffer, capacity65_7in_11out_nonblocking)
{
    ymn::ringbuffer<std::size_t> rb(65, true);

    std::thread producer {producer_Nelements<decltype(rb), 7>, std::ref(rb)};
    std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;
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
template<typename RB>
static void producer_1element(RB& rb)
{
    long status;
    std::size_t produced;
    std::size_t wouldblock_cnt;
    typename RB::value_type element;

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < ITERATIONS) {
        element = produced;
        status = rb.write(element);
        if (status < 0) {
            if (static_cast<long>(ymn::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.write() failed with code " << status << std::endl;
                break;
            }
        }
        if (status != 1) {
            std::cout << "rb.write() returned " << status << "whereas expected was 1" << std::endl;
            break;
        }
        produced += status;
        //std::cout << "rb.write() returned " << status << " produced " << produced << std::endl;
    }
    mutex.lock();
    std::cout << "produced: " << produced << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}

template<typename RB>
static void consumer_1element(RB& rb)
{
    long status;
    std::size_t consumed;
    std::size_t wouldblock_cnt;
    typename RB::value_type element;

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < ITERATIONS) {
        element = -1;
        status = rb.read(element);
        if (status < 0) {
            if (static_cast<long>(ymn::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.read() failed with code " << status << std::endl;
                break;
            }
        }
        if (status != 1) {
            std::cout << "rb.read() returned " << status << " whereas expected was 1" << std::endl;
            break;
        }
        if (element != consumed) {
            std::cout << "read element " << element << " does not match with consumed elements " << consumed << std::endl;
            break;
        }
        consumed += status;
        //std::cout << "rb.read() returned " << status << " consumed " << consumed << std::endl;
    }
    mutex.lock();
    std::cout << "consumed: " << consumed << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}

template<typename RB, std::size_t N>
static void producer_Nelements(RB& rb)
{
    long status;
    std::size_t produced;
    std::size_t wouldblock_cnt;
    typename RB::value_type array[N];

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < ITERATIONS) {
        memset(array, 0, sizeof(array));
        for (std::size_t j = 0; j < (sizeof(array) / sizeof(array[0])); ++j)
            array[j] = produced + j;
        status = rb.write(array);
        if (status < 0) {
            if (static_cast<long>(ymn::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.write() failed with code " << status << std::endl;
                break;
            }
        }
        produced += status;
        //std::cout << "rb.write() returned " << status << " produced " << produced << std::endl;
    }
    mutex.lock();
    std::cout << "produced: " << produced << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}

template<typename RB, std::size_t N>
static void consumer_Nelements(RB& rb)
{
    long status;
    std::size_t consumed;
    std::size_t wouldblock_cnt;
    typename RB::value_type array[N];

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < ITERATIONS) {
        memset(array, 0, sizeof(array));
        status = rb.read(array);
        if (status < 0) {
            if (static_cast<long>(ymn::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.read() failed with code " << status << std::endl;
                break;
            }
        }
        for (long i = 0; i < status; ++i) {
            if (array[i] != consumed + i) {
                std::cout << "error in order of elements in the array" << std::endl;
                break;
            }
        }
        consumed += status;
        //std::cout << "rb.read() returned " << status << " consumed " << consumed << std::endl;
    }
    mutex.lock();
    std::cout << "consumed: " << consumed << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}
