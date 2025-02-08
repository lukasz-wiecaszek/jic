/* SPDX-License-Identifier: MIT */
/**
 * @file iohandle.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _IOHANDLE_HPP_
#define _IOHANDLE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <utility>
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

template<typename T>
class iohandle {
public:
    explicit iohandle(std::function<bool(T)> callback = nullptr)
    : m_callback{callback}
    {
    }

    ~iohandle()
    {
    }

    // iohandle shall be non-copyable
    iohandle(const iohandle&) = delete;
    iohandle& operator=(const iohandle&) = delete;

    // but it shall be movable
    iohandle(iohandle&& other)
    : m_callback{std::exchange(other.m_callback, nullptr)}
    {
    }

    iohandle& operator=(iohandle&& other)
    {
        m_callback = std::exchange(other.m_callback, nullptr);
        return *this;
    }

    bool done(T data)
    {
        bool status = true;

        if (m_callback)
            status = m_callback(data);

        return status;
    };

private:
    std::function<bool(T)> m_callback;
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

#endif /* _IOHANDLE_HPP_ */
