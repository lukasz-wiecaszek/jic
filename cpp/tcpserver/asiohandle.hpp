/* SPDX-License-Identifier: MIT */
/**
 * @file asiohandle.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _ASIOHANDLE_HPP_
#define _ASIOHANDLE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <utility>
#include <functional>
#include <optional>
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
class asiohandle {
public:
    explicit asiohandle(asiohandle** hptr = nullptr, std::function<void(T)> callback = nullptr)
    : m_data{}
    , m_hptr{hptr}
    , m_callback{callback}
    , m_caller{}
    {
        if (m_hptr)
            *m_hptr = this;
    }

    explicit asiohandle(T data, asiohandle** hptr = nullptr, std::function<void(T)> callback = nullptr)
    : m_data{data}
    , m_hptr{hptr}
    , m_callback{callback}
    , m_caller{}
    {
        if (m_hptr)
            *m_hptr = this;
    }

    ~asiohandle()
    {
        if (m_hptr)
            *m_hptr = nullptr;
    }

    // asiohandle shall be non-copyable
    asiohandle(const asiohandle&) = delete;
    asiohandle& operator=(const asiohandle&) = delete;

    // but it shall be movable
    asiohandle(asiohandle&& other)
    : m_data{std::exchange(other.m_data, nullptr)}
    , m_callback{std::exchange(other.m_callback, nullptr)}
    , m_caller{std::exchange(other.m_caller, nullptr)}
    {
    }

    asiohandle& operator=(asiohandle&& other)
    {
        m_data = std::exchange(other.m_data, nullptr);
        m_callback = std::exchange(other.m_callback, nullptr);
        m_caller = std::exchange(other.m_caller, nullptr);
        return *this;
    }

    bool await_ready() const noexcept
    {
        return m_data.has_value();
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        m_caller = handle;
    }

    T await_resume() const noexcept
    {
        return m_data.value();
    }

    void done(T data)
    {
        if (m_callback)
            m_callback(data);
        m_data = data;
        if (m_caller)
            m_caller.resume();
    };

private:
    std::optional<T> m_data;
    asiohandle** m_hptr;
    std::function<void(T)> m_callback;
    std::coroutine_handle<> m_caller;
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

#endif /* _ASIOHANDLE_HPP_ */
