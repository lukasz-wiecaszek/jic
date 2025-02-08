/* SPDX-License-Identifier: MIT */
/**
 * @file coroutine_session.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _COROUTINE_SESSION_HPP_
#define _COROUTINE_SESSION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <memory>
#include <thread>
#include <string>
#include <functional>
#include <tuple>
#include <array>
#include <queue>
#include <optional>
#include <expected>

#include <sys/socket.h>
#include <netinet/in.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "session.hpp"
#include "iouring.hpp"
#include "charbuffer.hpp"
#include "iostatus.hpp"
#include "asiohandle.hpp"
#include "deferred.hpp"
#include "job.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define NUM_SQ_ENTRIES (128)

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

class coroutine_session
: public session
, public std::enable_shared_from_this<coroutine_session> {
public:
    coroutine_session(const session_args& args);
    ~coroutine_session() override;

    // coroutine_session shall be non-copyable and non-movable
    coroutine_session(const coroutine_session&) = delete;
    coroutine_session(coroutine_session&&) = delete;
    coroutine_session& operator=(const coroutine_session&) = delete;
    coroutine_session& operator=(coroutine_session&&) = delete;

    void terminate() override;

private:
    struct user_data {
        int index;
        asiohandle<iostatus>* asiohndl;
    };

    user_data* get_user_data()
    {
        if (m_free_slots.empty())
            return nullptr;

        int slot = m_free_slots.front();
        m_free_slots.pop();

        return &m_user_data[slot];
    }

    void put_user_data(user_data* ud)
    {
        m_free_slots.push(ud->index);
    }

    asiohandle<iostatus> schedule(io_uring_sqe* sqe);
    asiohandle<iostatus> schedule_read(charbuffer& buffer);
    asiohandle<iostatus> schedule_write(const char* line, std::size_t len);
    deferred<bool> read(charbuffer& buffer);
    deferred<bool> write(const std::string& outline);
    deferred<bool> worker();
    job setup_termination_handler(std::function<void()> handler);
    iostatus io_run();
    void thread_function();

private:
    iouring m_iouring;
    std::array<user_data, NUM_SQ_ENTRIES> m_user_data;
    std::queue<int> m_free_slots;
    std::thread m_thread;
    int m_pipefds[2];
    bool m_stop_requested;
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

#endif /* _COROUTINE_SESSION_HPP_ */
