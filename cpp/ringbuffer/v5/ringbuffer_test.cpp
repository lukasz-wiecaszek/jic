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
#include "gmock/gmock.h"
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

#define TEST_1IN_1OUT_BLOCKING(TYPE, CAPACITY)                                               \
TEST(ringbuffer, 1in_1out_type_##TYPE##_capacity_##CAPACITY##_blocking)                      \
{                                                                                            \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);              \
                                                                                             \
        std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};                \
        std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};                \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);              \
                                                                                             \
        std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};            \
        std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};            \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);              \
                                                                                             \
        std::thread producer {producer_function<decltype(rb), 1>, std::ref(rb)};             \
        std::thread consumer {consumer_function<decltype(rb), 1>, std::ref(rb)};             \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
}

#define TEST_1IN_1OUT_NONBLOCKING(TYPE, CAPACITY)                                            \
TEST(ringbuffer, 1in_1out_type_##TYPE##_capacity_##CAPACITY##_nonblocking)                   \
{                                                                                            \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);        \
                                                                                             \
        std::thread producer {producer_1element<decltype(rb)>, std::ref(rb)};                \
        std::thread consumer {consumer_1element<decltype(rb)>, std::ref(rb)};                \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);        \
                                                                                             \
        std::thread producer {producer_Nelements<decltype(rb), 1>, std::ref(rb)};            \
        std::thread consumer {consumer_Nelements<decltype(rb), 1>, std::ref(rb)};            \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);        \
                                                                                             \
        std::thread producer {producer_function<decltype(rb), 1>, std::ref(rb)};             \
        std::thread consumer {consumer_function<decltype(rb), 1>, std::ref(rb)};             \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
}

#define TEST_7IN_11OUT_BLOCKING(TYPE, CAPACITY)                                              \
TEST(ringbuffer, 7in_11out_type_##TYPE##_capacity_##CAPACITY##_blocking)                     \
{                                                                                            \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);              \
                                                                                             \
        std::thread producer {producer_Nelements<decltype(rb),  7>, std::ref(rb)};           \
        std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};           \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);              \
                                                                                             \
        std::thread producer {producer_function<decltype(rb),  7>, std::ref(rb)};            \
        std::thread consumer {consumer_function<decltype(rb), 11>, std::ref(rb)};            \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
}

#define TEST_7IN_11OUT_NONBLOCKING(TYPE, CAPACITY)                                           \
TEST(ringbuffer, 7in_11out_type_##TYPE##_capacity_##CAPACITY##_nonblocking)                  \
{                                                                                            \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);        \
                                                                                             \
        std::thread producer {producer_Nelements<decltype(rb),  7>, std::ref(rb)};           \
        std::thread consumer {consumer_Nelements<decltype(rb), 11>, std::ref(rb)};           \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
    {                                                                                        \
        lts::ringbuffer<TYPE> rb(CAPACITY, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);        \
                                                                                             \
        std::thread producer {producer_function<decltype(rb),  7>, std::ref(rb)};            \
        std::thread consumer {consumer_function<decltype(rb), 11>, std::ref(rb)};            \
                                                                                             \
        producer.join();                                                                     \
        consumer.join();                                                                     \
                                                                                             \
        std::cout << static_cast<std::string>(rb) << std::endl;                              \
    }                                                                                        \
}

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

struct rb_element
{
    rb_element() :
        m_value(-1)
    {
    }

    rb_element(long value) :
        m_value{value}
    {
    }

    ~rb_element()
    {
    }

    rb_element(const rb_element& other)
    {
        m_value = other.m_value;
    }

    rb_element(rb_element&& other)
    {
        m_value = other.m_value;
    }

    rb_element& operator = (const rb_element& other)
    {
        m_value = other.m_value;
        return *this;
    }

    rb_element& operator = (rb_element&& other)
    {
        m_value = other.m_value;
        return *this;
    }

    bool operator == (const rb_element& other) const
    {
        return m_value == other.m_value;
    }

    bool operator != (const rb_element& other) const
    {
        return m_value != other.m_value;
    }

    friend std::string to_string(const rb_element& e);

    long m_value;
};

inline std::string to_string(const rb_element& e)
{
    return to_string(e.m_value);
}

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/
template<typename RB> static void producer_1element(RB& rb);
template<typename RB> static void consumer_1element(RB& rb);
template<typename RB, size_t N> static void producer_Nelements(RB& rb);
template<typename RB, size_t N> static void consumer_Nelements(RB& rb);
template<typename RB, size_t N> static void producer_function(RB& rb);
template<typename RB, size_t N> static void consumer_function(RB& rb);

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

TEST(ringbuffer, create_on_stack_primitive_type)
{
    const size_t capacity = 1;
    lts::ringbuffer<size_t> rb1(capacity, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);
    lts::ringbuffer<size_t> rb2(capacity, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);

    EXPECT_EQ(capacity, rb1.capacity());
    EXPECT_EQ(capacity, rb2.capacity());

    EXPECT_EQ(RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING, rb1.flags());
    EXPECT_EQ(RINGBUFFER_RD_BLOCKING_WR_BLOCKING, rb2.flags());

    std::cout << static_cast<std::string>(rb1) << std::endl;
    std::cout << static_cast<std::string>(rb2) << std::endl;
}

TEST(ringbuffer, create_on_stack_class_type)
{
    const size_t capacity = 1;

    lts::ringbuffer<rb_element> rb1(capacity, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);
    lts::ringbuffer<rb_element> rb2(capacity, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);

    EXPECT_EQ(capacity, rb1.capacity());
    EXPECT_EQ(capacity, rb2.capacity());

    EXPECT_EQ(RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING, rb1.flags());
    EXPECT_EQ(RINGBUFFER_RD_BLOCKING_WR_BLOCKING, rb2.flags());

    std::cout << static_cast<std::string>(rb1) << std::endl;
    std::cout << static_cast<std::string>(rb2) << std::endl;
}

TEST(ringbuffer, create_on_heap_primitive_type)
{
    const size_t capacity = 1;

    lts::ringbuffer<size_t> *rb1 = new lts::ringbuffer<size_t>(capacity, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);
    lts::ringbuffer<size_t> *rb2 = new lts::ringbuffer<size_t>(capacity, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);

    EXPECT_EQ(capacity, rb1->capacity());
    EXPECT_EQ(capacity, rb2->capacity());

    EXPECT_EQ(RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING, rb1->flags());
    EXPECT_EQ(RINGBUFFER_RD_BLOCKING_WR_BLOCKING, rb2->flags());

    std::cout << rb1->to_string() << std::endl;
    std::cout << rb2->to_string() << std::endl;

    delete rb1;
    delete rb2;
}

TEST(ringbuffer, create_on_heap_class_type)
{
    const size_t capacity = 1;

    lts::ringbuffer<rb_element> *rb1 = new lts::ringbuffer<rb_element>(capacity, RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING);
    lts::ringbuffer<rb_element> *rb2 = new lts::ringbuffer<rb_element>(capacity, RINGBUFFER_RD_BLOCKING_WR_BLOCKING);

    EXPECT_EQ(capacity, rb1->capacity());
    EXPECT_EQ(capacity, rb2->capacity());

    EXPECT_EQ(RINGBUFFER_RD_NONBLOCKING_WR_NONBLOCKING, rb1->flags());
    EXPECT_EQ(RINGBUFFER_RD_BLOCKING_WR_BLOCKING, rb2->flags());

    std::cout << rb1->to_string() << std::endl;
    std::cout << rb2->to_string() << std::endl;

    delete rb1;
    delete rb2;
}

/*===========================================================================*\
 * tests of blocking semantic of ringbuffer
\*===========================================================================*/
TEST_1IN_1OUT_BLOCKING(size_t,  1)
TEST_1IN_1OUT_BLOCKING(size_t,  2)
TEST_1IN_1OUT_BLOCKING(size_t, 64)
TEST_1IN_1OUT_BLOCKING(size_t, 65)

TEST_7IN_11OUT_BLOCKING(rb_element,  1)
TEST_7IN_11OUT_BLOCKING(rb_element,  2)
TEST_7IN_11OUT_BLOCKING(rb_element, 64)
TEST_7IN_11OUT_BLOCKING(rb_element, 65)

/*===========================================================================*\
 * tests of non-blocking semantic of ringbuffer
\*===========================================================================*/
TEST_1IN_1OUT_NONBLOCKING(size_t,  1)
TEST_1IN_1OUT_NONBLOCKING(size_t,  2)
TEST_1IN_1OUT_NONBLOCKING(size_t, 64)
TEST_1IN_1OUT_NONBLOCKING(size_t, 65)

TEST_7IN_11OUT_NONBLOCKING(rb_element,  1)
TEST_7IN_11OUT_NONBLOCKING(rb_element,  2)
TEST_7IN_11OUT_NONBLOCKING(rb_element, 64)
TEST_7IN_11OUT_NONBLOCKING(rb_element, 65)

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
    size_t produced;
    size_t wouldblock_cnt;
    typename RB::value_type element;

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < ITERATIONS) {
        element = produced;
        status = rb.write(element);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
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
    size_t consumed;
    size_t wouldblock_cnt;
    typename RB::value_type element;

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < ITERATIONS) {
        element = -1;
        status = rb.read(element);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
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
            std::cout << "read element " << to_string(element) << " does not match with consumed elements " << consumed << std::endl;
            break;
        }
        consumed += status;
        //std::cout << "rb.read() returned " << status << " consumed " << consumed << std::endl;
    }
    mutex.lock();
    std::cout << "consumed: " << consumed << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}

template<typename RB, size_t N>
static void producer_Nelements(RB& rb)
{
    long status;
    size_t produced;
    size_t wouldblock_cnt;
    typename RB::value_type array[N];

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < ITERATIONS) {
        memset(array, 0, sizeof(array));
        for (size_t j = 0; j < (sizeof(array) / sizeof(array[0])); ++j)
            array[j] = produced + j;
        status = rb.write(array);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
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

template<typename RB, size_t N>
static void consumer_Nelements(RB& rb)
{
    long status;
    size_t consumed;
    size_t wouldblock_cnt;
    typename RB::value_type array[N];

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < ITERATIONS) {
        memset(array, 0, sizeof(array));
        status = rb.read(array);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
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

template<typename RB, size_t N>
static void producer_function(RB& rb)
{
    long status;
    size_t produced;
    size_t wouldblock_cnt;

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < ITERATIONS) {
        status = rb.write([&produced](typename RB::value_type *dst) -> bool {
            *dst = produced++;
            return true;
        }, N);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.write() failed with code " << status << std::endl;
                break;
            }
        }
        //std::cout << "rb.write() returned " << status << " produced " << produced << std::endl;
    }
    mutex.lock();
    std::cout << "produced: " << produced << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}

template<typename RB, size_t N>
static void consumer_function(RB& rb)
{
    long status;
    size_t consumed;
    size_t wouldblock_cnt;

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < ITERATIONS) {
        status = rb.read([&consumed](typename RB::value_type *src) -> bool {
            if (*src == consumed++)
                return true;
            else {
                std::cout << "error in order of elements" << std::endl;
                return false;
            }
        }, N);
        if (status < 0) {
            if (static_cast<long>(lts::ringbuffer_status::WOULD_BLOCK) == status) {
                wouldblock_cnt++;
                continue;
            }
            else {
                std::cout << "rb.read() failed with code " << status << std::endl;
                break;
            }
        }
        //std::cout << "rb.read() returned " << status << " consumed " << consumed << std::endl;
    }
    mutex.lock();
    std::cout << "consumed: " << consumed << " wouldblock_cnt: " << wouldblock_cnt << std::endl;
    mutex.unlock();
}
