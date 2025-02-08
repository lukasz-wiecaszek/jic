/* SPDX-License-Identifier: MIT */
/**
 * @file iouring_session.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _IOURING_SESSION_HPP_
#define _IOURING_SESSION_HPP_

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

#include <sys/socket.h>
#include <netinet/in.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "session.hpp"
#include "iouring.hpp"
#include "charbuffer.hpp"
#include "iostatus.hpp"
#include "iohandle.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define NUM_SQ_ENTRIES (128)

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

class iouring_session
: public session
, public std::enable_shared_from_this<iouring_session> {
public:
    iouring_session(const session_args& args);
    ~iouring_session() override;

    // iouring_session shall be non-copyable and non-movable
    iouring_session(const iouring_session&) = delete;
    iouring_session(iouring_session&&) = delete;
    iouring_session& operator=(const iouring_session&) = delete;
    iouring_session& operator=(iouring_session&&) = delete;

    void terminate() override;

private:
    struct user_data {
        int index;
        std::unique_ptr<iohandle<iostatus>> iohndl;
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
        if (ud->iohndl != nullptr)
            ud->iohndl.release();

        m_free_slots.push(ud->index);
    }

    iostatus schedule(io_uring_sqe* sqe, std::function<bool(iostatus status)> callback);
    iostatus schedule_read(charbuffer& buffer);
    iostatus schedule_write(const std::string& outline);
    bool handle_read(iostatus status, charbuffer& buffer);
    bool handle_write(iostatus status, std::string outline);
    iostatus setup_termination_handler();
    iostatus io_run();
    void thread_function();

private:
    iouring m_iouring;
    std::array<user_data, NUM_SQ_ENTRIES> m_user_data;
    std::queue<int> m_free_slots;
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

#endif /* _IOURING_SESSION_HPP_ */
