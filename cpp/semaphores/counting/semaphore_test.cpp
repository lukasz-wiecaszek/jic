/**
 * @file semaphore_test.cpp
 *
 * Test procedures for 'semaphore' type.
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
#include "semaphore.hpp"

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

TEST(semaphore, create_on_stack)
{
    ymn::semaphore sem(3);

    ASSERT_EQ(3, sem.get_value());

    while (sem.get_value() > 0)
        sem.wait();

    std::thread t1 {[](ymn::semaphore* sem){
        sem->post();
        sem->wait();
        sem->post();
        sem->wait_timeout(WAIT_TIMEOUT_MSEC);
        sem->post();
    }, &sem};

    std::thread t2 {[](ymn::semaphore& sem){
        sem.wait();
        sem.post();
        sem.wait_timeout(WAIT_TIMEOUT_MSEC);
        sem.post();
        sem.wait();
    }, std::ref(sem)};

    t1.join();
    t2.join();
}

TEST(semaphore, create_on_heap)
{
    ymn::semaphore *sem = new (std::nothrow) ymn::semaphore(3);

    ASSERT_EQ(3, sem->get_value());

    while (sem->get_value() > 0)
        sem->wait();

    std::thread t1 {[](ymn::semaphore* sem){
        sem->post();
        sem->wait();
        sem->post();
        sem->wait_timeout(WAIT_TIMEOUT_MSEC);
        sem->post();
    }, sem};

    std::thread t2 {[](ymn::semaphore& sem){
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

TEST(semaphore, wait_interrupted)
{
    ymn::semaphore sem(3);

    ASSERT_EQ(3, sem.get_value());

    while (sem.get_value() > 0)
        sem.wait();

    std::thread t1 {[](ymn::semaphore* sem){
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIMEOUT_MSEC / 2));
        sem->post();
    }, &sem};

    sem.wait();

    t1.join();
}

TEST(semaphore, wait_timeout_interrupted)
{
    ymn::semaphore sem(3);

    ASSERT_EQ(3, sem.get_value());

    while (sem.get_value() > 0)
        EXPECT_TRUE(sem.wait_timeout(WAIT_TIMEOUT_MSEC));

    std::thread t1 {[](ymn::semaphore* sem){
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIMEOUT_MSEC / 2));
        sem->post();
    }, &sem};

    EXPECT_TRUE(sem.wait_timeout(WAIT_TIMEOUT_MSEC));

    t1.join();
}

TEST(semaphore, wait_timeout)
{
    ymn::semaphore sem(3);

    ASSERT_EQ(3, sem.get_value());

    while (sem.get_value() > 0)
        EXPECT_TRUE(sem.wait_timeout(WAIT_TIMEOUT_MSEC));

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
