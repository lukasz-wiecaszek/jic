/**
 * @file shared_ptr_ut.cpp
 *
 * Unit tests (UT) for 'shared_ptr'.
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

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
//#define USE_STD_SHARED_PTR

#if !defined(USE_STD_SHARED_PTR)
#include "shared_ptr.hpp"
using namespace lts;
#else
#include <memory>
using namespace std;
#endif

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

struct base
{
    base(int id) :
        m_id(id)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }
    virtual ~base()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
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
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }
    virtual ~derived()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }
};

struct deleter
{
    deleter()
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    void operator () (struct base* b)
    {
#if defined(DEBUG_SHARED_PTR)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
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
}

TEST(shared_ptr, swap_1)
{
    const int id1 = 1;
    shared_ptr<base> sptr1(new derived(id1));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(id1, sptr1->id());

    sptr1.swap(sptr1);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(id1, sptr1->id());
}

TEST(shared_ptr, swap_2)
{
    const int id1 = 1;
    shared_ptr<base> sptr1(new derived(id1));
    const int id2 = 2;
    shared_ptr<base> sptr2(new derived(id2));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(id1, sptr1->id());

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);
    EXPECT_EQ(id2, sptr2->id());

    sptr1.swap(sptr2);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(id2, sptr1->id());

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);
    EXPECT_EQ(id1, sptr2->id());
}

TEST(shared_ptr, dereference_1)
{
    derived* ptr = new derived(1);
    shared_ptr<base> sptr1(ptr);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(ptr, sptr1.get());
    EXPECT_TRUE(sptr1);
}

TEST(shared_ptr, dereference_2)
{
    derived* ptr = new derived(1);
    shared_ptr<base> sptr1(ptr);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(ptr, &(*sptr1));
    EXPECT_TRUE(sptr1);
}

TEST(shared_ptr, dereference_3)
{
    derived* ptr = new derived(1);
    shared_ptr<base> sptr1(ptr);

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);

    EXPECT_EQ(ptr->id(), sptr1->id());
    EXPECT_TRUE(sptr1);
}

#if !defined(USE_STD_SHARED_PTR)

TEST(shared_ptr, to_sting)
{
    shared_ptr<base> sptr;

    EXPECT_EQ(static_cast<std::string>(sptr), sptr.to_string());
}

#endif

TEST(shared_ptr, multithreaded)
{
    const int counter = 1000;

    const int id1 = -1;
    shared_ptr<base> sptr1(new derived(id1));
    const int id2 = -2;
    shared_ptr<base> sptr2(new derived(id2));
    const int id3 = -3;
    shared_ptr<base> sptr3(new derived(id3));

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(id1, sptr1->id());

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);
    EXPECT_EQ(id2, sptr2->id());

    EXPECT_EQ(1, sptr3.use_count());
    EXPECT_TRUE(sptr3);
    EXPECT_EQ(id3, sptr3->id());

    std::thread t1 {[&](){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            EXPECT_EQ(loop_cnt, sptr->id());

            sptr1.swap(sptr);
            EXPECT_EQ(loop_cnt, sptr1->id());
        }
    }};

    std::thread t2 {[&](){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            EXPECT_EQ(loop_cnt, sptr->id());

            sptr2.swap(sptr);
            EXPECT_EQ(loop_cnt, sptr2->id());
        }
    }};

    std::thread t3 {[&](){
        for (int loop_cnt = 0; loop_cnt < counter; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            EXPECT_EQ(loop_cnt, sptr->id());

            sptr3.swap(sptr);
            EXPECT_EQ(loop_cnt, sptr3->id());
        }
    }};

    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(1, sptr1.use_count());
    EXPECT_TRUE(sptr1);
    EXPECT_EQ(counter - 1, sptr1->id());

    EXPECT_EQ(1, sptr2.use_count());
    EXPECT_TRUE(sptr2);
    EXPECT_EQ(counter - 1, sptr2->id());

    EXPECT_EQ(1, sptr3.use_count());
    EXPECT_TRUE(sptr3);
    EXPECT_EQ(counter - 1, sptr3->id());
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
