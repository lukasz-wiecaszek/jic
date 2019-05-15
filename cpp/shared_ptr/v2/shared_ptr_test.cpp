/**
 * @file shared_ptr_test.cpp
 *
 * Test procedures for 'shared_ptr' type.
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

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "gtest/gtest.h"
#include "shared_ptr.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace ymn;
//using namespace std;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
/* Because the noexcept operator always returns true for a constant expression,
it can be used to check if a particular invocation of a constexpr
takes the constant expression branch. */
#define IS_CONSTEXPR(expr) noexcept(expr)

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

struct base : public ref_base<base>
{
    base(int id) :
        m_id(id)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    virtual ~base()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    int id() const
    {
        return m_id;
    }

private:
    int m_id;
};

struct derived : public base
{
    derived(int id) :
        base(id)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    virtual ~derived()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

struct deleter
{
    deleter()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void operator () (struct base* b)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        delete b;
    }
};

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

#include "constructors.test"
#include "copy_assigments.test"












#if 0

TEST(shared_ptr, reset_1)
{
    shared_ptr<derived> sptr1(new derived(1));
    shared_ptr<base> sptr2(sptr1);

    EXPECT_EQ(2, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(2, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    sptr1.reset();

    EXPECT_EQ(0, sptr1.use_count());
    EXPECT_FALSE(sptr1);

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    std::cout << (std::string)sptr1 << std::endl;
    std::cout << (std::string)sptr2 << std::endl;
}

TEST(shared_ptr, reset_2)
{
    shared_ptr<derived> sptr1(new derived(1));
    shared_ptr<base> sptr2(sptr1);

    EXPECT_EQ(2, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(2, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    sptr1.reset(new derived(2));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    std::cout << (std::string)sptr1 << std::endl;
    std::cout << (std::string)sptr2 << std::endl;
}

TEST(shared_ptr, reset_3)
{
    shared_ptr<derived> sptr1(new derived(1));
    shared_ptr<base> sptr2(sptr1);

    EXPECT_EQ(2, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(2, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    sptr1.reset(new derived(2), deleter());

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    std::cout << (std::string)sptr1 << std::endl;
    std::cout << (std::string)sptr2 << std::endl;
}

TEST(shared_ptr, swap_1)
{
    shared_ptr<base> sptr1(new derived(1));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    sptr1.swap(sptr1);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    std::cout << (std::string)sptr1 << std::endl;
}

TEST(shared_ptr, swap_2)
{
    shared_ptr<base> sptr1(new derived(1));
    shared_ptr<base> sptr2(new derived(2));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    sptr1.swap(sptr2);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);

    std::cout << (std::string)sptr1 << std::endl;
    std::cout << (std::string)sptr2 << std::endl;
}

TEST(shared_ptr, dereference_1)
{
    shared_ptr<base> sptr1(new derived(1));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    base b = *sptr1;

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    std::cout << (std::string)sptr1 << std::endl;
}

TEST(shared_ptr, dereference_2)
{
    shared_ptr<base> sptr1(new derived(1));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(1, sptr1->id());

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    std::cout << (std::string)sptr1 << std::endl;
}

TEST(shared_ptr, multithreaded_1)
{
    const int counter = 3;
    shared_ptr<base, std::atomic<int>> sptr(new derived(1));

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    EXPECT_EQ(1, sptr->id());

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    std::cout << (std::string)sptr << std::endl;

    std::thread t1 {[sptr1 = sptr](){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    std::thread t2 {[sptr1 = sptr] (){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    std::thread t3 {[sptr1 = sptr] (){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    t1.join();
    t2.join();
	t3.join();

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    std::cout << (std::string)sptr << std::endl;
}

TEST(shared_ptr, multithreaded_2)
{
    const int counter = 100;
    shared_ptr<base, std::atomic<int>> sptr(new derived(1));

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    EXPECT_EQ(1, sptr->id());

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    std::cout << (std::string)sptr << std::endl;

    std::thread t1 {[sptr1 = sptr](){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    std::thread t2 {[sptr1 = sptr] (){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    std::thread t3 {[sptr1 = sptr] (){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            decltype(sptr1) sptr2(sptr1); /* copy constructor and then destructor */
            std::cout << (std::string)sptr2 << std::endl;
        }
    }};

    t1.join();
    t2.join();
	t3.join();

    EXPECT_EQ(1, sptr.use_count());
    EXPECT_TRUE(sptr);

    std::cout << (std::string)sptr << std::endl;
}
#endif

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
