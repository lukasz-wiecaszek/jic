/* SPDX-License-Identifier: MIT */
/**
 * @file session.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _SESSION_HPP_
#define _SESSION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <memory>
#include <functional>

#include <sys/socket.h>
#include <netinet/in.h>

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

class session;

struct session_args {
    int sockfd;
    struct sockaddr_in addr;
    std::function<void(std::shared_ptr<session> session)> release;
};

class session {
public:
    session(const session_args& args);
    virtual ~session();

    // session shall be non-copyable and non-movable
    session(const session&) = delete;
    session(session&&) = delete;
    session& operator=(const session&) = delete;
    session& operator=(session&&) = delete;

    bool is_valid() const
    {
        return m_state == state_e::initialized;
    }

    operator bool() const
    {
        return is_valid();
    }

    std::string to_string() const;

    operator std::string() const
    {
        return to_string();
    }

    virtual void terminate() = 0;

protected:
    enum class state_e {
        uninitialized,
        initialized
    } m_state;
    session_args m_args;
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

#endif /* _SESSION_HPP_ */
