/**
 * @file shared_ptr_pt.cpp
 *
 * Performance tests (PT) for 'shared_ptr'.
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
#define ITERATIONS 100000000

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
static void shared_ptr_pt_multithreaded();

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> t1, t2;

    std::cout << "test started ..." << std::endl;

    t1 = std::chrono::high_resolution_clock::now();

    shared_ptr_pt_multithreaded();

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
static void shared_ptr_pt_multithreaded()
{
    const int id1 = -1;
    shared_ptr<base> sptr1(new derived(id1));
    const int id2 = -2;
    shared_ptr<base> sptr2(new derived(id2));
    const int id3 = -3;
    shared_ptr<base> sptr3(new derived(id3));

    std::thread t1 {[&](){
        for (int loop_cnt = 0; loop_cnt < ITERATIONS; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            sptr1.swap(sptr);
        }
    }};

    std::thread t2 {[&](){
        for (int loop_cnt = 0; loop_cnt < ITERATIONS; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            sptr2.swap(sptr);
        }
    }};

    std::thread t3 {[&](){
        for (int loop_cnt = 0; loop_cnt < ITERATIONS; ++loop_cnt) {
            shared_ptr<base> sptr(new derived(loop_cnt));
            sptr3.swap(sptr);
        }
    }};

    t1.join();
    t2.join();
    t3.join();
}
