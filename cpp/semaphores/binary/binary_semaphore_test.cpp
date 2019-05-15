/**
 * @file binary_semaphore_test.cpp
 *
 * Test procedures for 'binary_semaphore' type.
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
#include <chrono>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "binary_semaphore.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define WAIT_TIMEOUT_MSEC 200

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

TEST(binary_semaphore, create_on_stack)
{
    ymn::binary_semaphore sem(false);

    ASSERT_EQ(false, sem.get_value());

    std::thread t1 {[](ymn::binary_semaphore* sem){
        sem->post();
        sem->wait();
        sem->post();
        sem->wait_timeout(WAIT_TIMEOUT_MSEC);
        sem->post();
    }, &sem};

    std::thread t2 {[](ymn::binary_semaphore& sem){
        sem.wait();
        sem.post();
        sem.wait_timeout(WAIT_TIMEOUT_MSEC);
        sem.post();
        sem.wait();
    }, std::ref(sem)};

    t1.join();
    t2.join();
}

TEST(binary_semaphore, create_on_heap)
{
    ymn::binary_semaphore *sem = new (std::nothrow) ymn::binary_semaphore(false);

    ASSERT_EQ(false, sem->get_value());

    std::thread t1 {[](ymn::binary_semaphore* sem){
        sem->post();
        sem->wait();
        sem->post();
        sem->wait_timeout(WAIT_TIMEOUT_MSEC);
        sem->post();
    }, sem};

    std::thread t2 {[](ymn::binary_semaphore& sem){
        sem.wait();
        sem.post();
        sem.wait_timeout(WAIT_TIMEOUT_MSEC);
        sem.post();
        sem.wait();
    }, std::ref(*sem)};

    t1.join();
    t2.join();

    delete sem;
}

TEST(binary_semaphore, wait_interrupted)
{
    ymn::binary_semaphore sem(false);

    ASSERT_EQ(false, sem.get_value());

    std::thread t1 {[](ymn::binary_semaphore* sem){
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIMEOUT_MSEC / 2));
        sem->post();
    }, &sem};

    sem.wait();

    t1.join();
}

TEST(binary_semaphore, wait_timeout_interrupted)
{
    ymn::binary_semaphore sem(false);

    ASSERT_EQ(false, sem.get_value());

    std::thread t1 {[](ymn::binary_semaphore* sem){
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIMEOUT_MSEC / 2));
        sem->post();
    }, &sem};

    EXPECT_TRUE(sem.wait_timeout(WAIT_TIMEOUT_MSEC));

    t1.join();
}

TEST(binary_semaphore, wait_timeout)
{
    ymn::binary_semaphore sem(false);

    ASSERT_EQ(false, sem.get_value());

    EXPECT_FALSE(sem.wait_timeout(WAIT_TIMEOUT_MSEC));
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
