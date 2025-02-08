/* SPDX-License-Identifier: MIT */
/**
 * @file oldschool_session.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _OLDSCHOOL_SESSION_HPP_
#define _OLDSCHOOL_SESSION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <memory>
#include <thread>
#include <string>
#include <functional>

#include <sys/socket.h>
#include <netinet/in.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "session.hpp"
#include "charbuffer.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

class oldschool_session
: public session
, public std::enable_shared_from_this<oldschool_session> {
public:
    oldschool_session(const session_args& args);
    ~oldschool_session() override;

    // oldschool_session shall be non-copyable and non-movable
    oldschool_session(const oldschool_session&) = delete;
    oldschool_session(oldschool_session&&) = delete;
    oldschool_session& operator=(const oldschool_session&) = delete;
    oldschool_session& operator=(oldschool_session&&) = delete;

    void terminate() override;

private:
    bool read(int sockfd, charbuffer& buffer);
    bool write(int sockfd, const std::string& outline);
    bool handler(int sockfd, charbuffer& buffer);
    void thread_function();

private:
    std::thread m_thread;
    int m_pipefds[2];
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

#endif /* _OLDSCHOOL_SESSION_HPP_ */
