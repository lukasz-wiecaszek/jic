/* SPDX-License-Identifier: MIT */
/**
 * @file job.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _JOB_HPP_
#define _JOB_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <coroutine>
#include <functional>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

struct job {
    struct promise_type {
        promise_type() noexcept {}
        ~promise_type() noexcept {}

        job get_return_object() noexcept { return {}; }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() noexcept {}
        void return_void() noexcept {}
    };
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

template <typename Awaitable>
job launch(Awaitable awaitable, std::function<void()> completion)
{
    co_await awaitable; completion();
}

} /* end of namespace lts */

/*===========================================================================*\
 * global (external linkage) objects declarations
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations (external linkage)
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

#endif /* _JOB_HPP_ */
