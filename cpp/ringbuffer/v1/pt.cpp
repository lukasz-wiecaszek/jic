/**
 * @file pt.cpp
 *
 * Performance tests for ringbuffer.
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
#include <chrono>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "ringbuffer.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CAPACITY   1000
#define ITERATIONS 100000000

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
template<typename RB, std::size_t N> static void producer_function(RB& rb, std::size_t iterations);
template<typename RB, std::size_t N> static void consumer_function(RB& rb, std::size_t iterations);

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
int main()
{
    std::size_t iterations = ITERATIONS;
    std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;

    std::cout << "test started ..." << std::endl;

    t1 = std::chrono::high_resolution_clock::now();

    lts::ringbuffer<std::size_t, CAPACITY, false> rb;

    std::thread producer {producer_function<decltype(rb), 1>, std::ref(rb), iterations};
    std::thread consumer {consumer_function<decltype(rb), 1>, std::ref(rb), iterations};

    producer.join();
    consumer.join();

    std::cout << static_cast<std::string>(rb) << std::endl;

    t2 = std::chrono::high_resolution_clock::now();
    uint64_t duration = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    std::cout << "test took " << duration << "ms" << std::endl;

    return 0;
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
template<typename RB, std::size_t N>
static void producer_function(RB& rb, std::size_t iterations)
{
    long status;
    std::size_t produced;
    std::size_t wouldblock_cnt;
    typename RB::value_type array[N];

    produced = 0;
    wouldblock_cnt = 0;
    while (produced < iterations) {
        memset(array, 0, sizeof(array));
        for (std::size_t j = 0; j < (sizeof(array) / sizeof(array[0])); ++j)
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
    }
    mutex.lock();
    std::cout << "iterations: " << iterations;
    std::cout << " produced: " << produced;
    std::cout << " wouldblock_cnt: " << wouldblock_cnt;
    std::cout << std::endl;
    mutex.unlock();
}

template<typename RB, std::size_t N>
static void consumer_function(RB& rb, std::size_t iterations)
{
    long status;
    std::size_t consumed;
    std::size_t wouldblock_cnt;
    typename RB::value_type array[N];

    consumed = 0;
    wouldblock_cnt = 0;
    while (consumed < iterations) {
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
    }
    mutex.lock();
    std::cout << "iterations: " << iterations;
    std::cout << " consumed: " << consumed;
    std::cout << " wouldblock_cnt: " << wouldblock_cnt;
    std::cout << std::endl;
    mutex.unlock();
}

