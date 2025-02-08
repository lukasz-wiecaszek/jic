/* SPDX-License-Identifier: MIT */
/**
 * @file deferred.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _DEFERRED_HPP_
#define _DEFERRED_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <utility>
#include <coroutine>

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

template<typename T>
struct deferred {
    struct promise_type {
        promise_type() noexcept {}
        ~promise_type() noexcept {}

        // Invoked when we first enter a coroutine.
        deferred get_return_object() noexcept
        {
            return deferred{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Called just before the coroutine body starts.
        std::suspend_never initial_suspend() const noexcept
        {
            return {};
        }

        struct awaiter {
            bool await_ready() const noexcept
            {
                return false;
            }

            auto await_suspend(std::coroutine_handle<promise_type> handle) noexcept
            {
                auto caller = handle.promise().get_caller();
                return caller ? caller : std::noop_coroutine();
            }

            void await_resume() const noexcept
            {
            }
        };

        // Called just after the coroutine body finishes.
        awaiter final_suspend() const noexcept
        {
            return {};
        }

        // If an exception was thrown in the coroutine body, it will be handled here.
        void unhandled_exception()
        {
        }

        // When the coroutine co_returns a value, this method is used to publish the result.
        void return_value(T value) noexcept
        {
            m_data = std::move(value);
        }

        // Sets a caller's coroutine handle.
        void set_caller(std::coroutine_handle<> caller) noexcept
        {
            m_caller = caller;
        }

        // Gets a caller's coroutine handle.
        std::coroutine_handle<> get_caller() const noexcept
        {
            return m_caller;
        }

        T data() const noexcept
        {
            return m_data;
        }

    private:
        // Keeps a coroutine handle referring to the parent/caller coroutine (if any).
        // That is, if we co_await a coroutine B from within a coroutine A,
        // this handle will be refering to coroutine A (caller) and will be used
        // to continue working in the coroutine A from where we left off.
        std::coroutine_handle<> m_caller;

        // Place to hold the results produced by the coroutine.
        T m_data;
    }; // promise_type

    explicit deferred(std::coroutine_handle<promise_type> handle)
    : m_self{handle}
    {
    }

    ~deferred()
    {
        if (m_self)
            m_self.destroy();
    }

    // deferred shall be non-copyable
    deferred(const deferred&) = delete;
    deferred& operator=(const deferred&) = delete;

    // but it shall be movable
    deferred(deferred&& other)
    : m_self(std::exchange(other.m_self, nullptr))
    {
    }

    deferred& operator=(deferred&& other)
    {
        if (m_self)
            m_self.destroy();
        m_self = std::exchange(other.m_self, nullptr);
        return *this;
    }

    // The following awaiter makes our deferred conformant to the awaitable concept,
    // so that we can co_await for that deferred.
    struct awaiter {
        bool await_ready() const noexcept
        {
            return handle.done();
        }

        void await_suspend(std::coroutine_handle<> caller) const noexcept
        {
            handle.promise().set_caller(caller);
        }

        T await_resume() const noexcept
        {
            return std::move(handle.promise().data());
        }

        std::coroutine_handle<promise_type> handle;
    };

    awaiter operator co_await() noexcept
    {
        // C++20 designated initializer syntax allows for that.
        return {.handle = m_self};
    }

    T get() const
    {
        return m_self.promise().data();
    }

    T operator*() const
    {
        return get();
    }

    operator T() const
    {
        return get();
    }

private:
    std::coroutine_handle<promise_type> m_self;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{
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

#endif /* _DEFERRED_HPP_ */
