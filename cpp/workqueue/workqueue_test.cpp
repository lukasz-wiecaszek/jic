/**
 * @file workqueuemgr_test.cpp
 *
 * Test procedures for 'workqueuemgr' type.
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
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "workqueue.hpp"

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

class completion
{
public:
    explicit completion(std::size_t count = 0) :
       m_mutex(),
       m_condvar(),
       m_count(count)
    {
    }

    ~completion() = default;

    completion(const completion&) = delete;
    completion(const completion&&) = delete;

    completion& operator = (const completion&) = delete;
    completion& operator = (const completion&&) = delete;

    int count() const
    {
        return m_count;
    }

    void done()
    {
        do {
            std::lock_guard<decltype(m_mutex)> lock(m_mutex);
            m_count--;
        } while (0);

        /* the lock does not need to be held for notification */
        m_condvar.notify_one();
    }

    void wait()
    {
        std::unique_lock<decltype(m_mutex)> lock(m_mutex);

        while (m_count > 0)
            m_condvar.wait(lock);
    }

    bool wait_timeout(unsigned int milliseconds)
    {
        const std::chrono::steady_clock::time_point t1(std::chrono::steady_clock::now());
        const std::chrono::milliseconds timeout(milliseconds);
        std::chrono::milliseconds elapsed(0);

        std::unique_lock<decltype(m_mutex)> lock(m_mutex);

        while (m_count > 0) {
            if (elapsed >= timeout)
                return false;

            if (std::cv_status::timeout == m_condvar.wait_for(lock, timeout - elapsed))
                return false;

            const std::chrono::steady_clock::time_point t2(std::chrono::steady_clock::now());
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        }

        return true;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condvar;
    int m_count;
};

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/
static int test_function(int a, int b, std::size_t sleep_time_msec, completion& cmpl);

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

TEST(workqueue, create_and_destroy)
{
    {
        ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 0);
        ASSERT_TRUE(wq.is_valid());
        std::cout << (std::string)wq << std::endl;
    }

    {
        ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 1);
        ASSERT_TRUE(wq.is_valid());
        std::cout << (std::string)wq << std::endl;
    }

    {
        ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 2);
        ASSERT_TRUE(wq.is_valid());
        std::cout << (std::string)wq << std::endl;
    }

    {
        ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 3);
        ASSERT_TRUE(wq.is_valid());
        std::cout << (std::string)wq << std::endl;
    }
}

TEST(workqueue, push_work_no_sleep_0_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 0;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 0);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_FALSE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_no_sleep_1_worker)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 0;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 1);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_no_sleep_2_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 0;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 2);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_no_sleep_3_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 0;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 3);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_with_sleep_0_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 100;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 0);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_FALSE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_with_sleep_1_worker)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 100;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 1);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_with_sleep_2_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 100;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 2);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
}

TEST(workqueue, push_work_with_sleep_3_workers)
{
    completion cmpl(10);
    const std::size_t sleep_time_msec = 100;

    ymn::workqueue wq(::testing::UnitTest::GetInstance()->current_test_info()->name(), 3);
    ASSERT_TRUE(wq.is_valid());
    std::cout << (std::string)wq << std::endl;

    for (int i = 0; i < cmpl.count(); ++i)
        wq.push_work(std::make_shared<ymn::work<int, int, int, completion&>>(test_function, i, i, sleep_time_msec, cmpl));

    EXPECT_TRUE(cmpl.wait_timeout(cmpl.count() * sleep_time_msec + 1000));
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
static int test_function(int a, int b, std::size_t sleep_time_msec, completion& cmpl)
{
    if (sleep_time_msec > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_msec));

    cmpl.done();

    return a + b;
}
