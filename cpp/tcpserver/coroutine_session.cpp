/* SPDX-License-Identifier: MIT */
/**
 * @file coroutine_session.cpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdio>
#include <cassert>
#include <chrono>
#include <expected>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "errnotostr.hpp"
#include "utilities.hpp"

#include "coroutine_session.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace lts;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * local types definitions
\*===========================================================================*/
namespace
{
} // end of anonymous namespace

/*===========================================================================*\
 * local (internal linkage) objects definitions
\*===========================================================================*/

/*===========================================================================*\
 * global (external linkage) objects definitions
\*===========================================================================*/

/*===========================================================================*\
 * local (internal linkage) functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class public functions definitions
\*===========================================================================*/
coroutine_session::coroutine_session(const session_args& args)
: session{args}
, std::enable_shared_from_this<coroutine_session>{}
, m_iouring{NUM_SQ_ENTRIES}
, m_user_data{}
, m_free_slots{}
, m_thread{}
, m_pipefds{INVALID_FD, INVALID_FD}
, m_stop_requested{false}
{
    do {
        int retval;

        if (!m_iouring.is_valid())
            break;

        for (int i = 0; i < NUM_SQ_ENTRIES; i++) {
            m_user_data[i].index = i;
            m_user_data[i].asiohndl = nullptr;
            m_free_slots.emplace(i);
        }

        retval = ::pipe(m_pipefds);
        assert("pipe()" && retval >= -1);
        if (retval == -1) {
            std::fprintf(stderr, "pipe() failed with code %d (%s)\n", errno, errnotostr(errno));
            break;
        }

        if (set_nonblocking_flag(m_pipefds[PIPE_READ_END]) != 0) {
            std::fprintf(stderr, "could not set pipe's reading end to non-blocking mode\n");
            break;
        }

        m_thread = std::thread(&coroutine_session::thread_function, this);
        m_thread.detach();

        std::fprintf(stdout, "[%s] coroutine_session created and initialized\n", to_string().c_str());
        m_state = state_e::initialized;
    } while (0);
}

coroutine_session::~coroutine_session()
{
    close_fd(m_pipefds[PIPE_READ_END]);
    close_fd(m_pipefds[PIPE_WRIRE_END]);

    std::fprintf(stdout, "[%s] coroutine_session destroyed\n", to_string().c_str());
}

void coroutine_session::terminate()
{
    // write any character to the pipe - this will wake poll on the pipe's reading end
    write_full(m_pipefds[PIPE_WRIRE_END], "!", 1);
}

/*===========================================================================*\
 * class protected functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class private functions definitions
\*===========================================================================*/
asiohandle<iostatus> coroutine_session::schedule(io_uring_sqe* sqe)
{
    if (sqe == nullptr) {
        std::fprintf(stderr, "sqe == nullptr\n");
        return asiohandle<iostatus>{EFAULT};
    }

    user_data* ud = get_user_data();
    if (ud == nullptr) {
        std::fprintf(stderr, "ud == nullptr\n");
        return asiohandle<iostatus>{ENODEV};
    }

    sqe->user_data = reinterpret_cast<uint64_t>(ud);

    int status = m_iouring.submit();
    if (status <= 0) {
        std::fprintf(stderr, "m_iouring.submit() failed\n");
        return asiohandle<iostatus>{-status};
    }

    return asiohandle<iostatus>{&ud->asiohndl, [this, ud](iostatus status) {
        put_user_data(ud);
    }};
}

asiohandle<iostatus> coroutine_session::schedule_read(charbuffer& buffer)
{
    return schedule(m_iouring.read(m_args.sockfd, buffer.write_ptr(), buffer.write_available()));
}

asiohandle<iostatus> coroutine_session::schedule_write(const char* line, std::size_t len)
{
    return schedule(m_iouring.write(m_args.sockfd, line, len));
}

deferred<bool> coroutine_session::read(charbuffer& buffer)
{
    bool retval = false;

    for (;;) {
        buffer.move();
        iostatus status = co_await schedule_read(buffer);
        if (status.has_value()) {
            if (status.value() > 0) {
                //std::fprintf(stderr, "Received %d bytes\n", status.value());
                buffer.produce(status.value());
                retval = true;
            } else {
                std::fprintf(stderr, "[%s] removing client from being served - connection closed!\n", to_string().c_str());
            }
            break;
        } else if (status.error() == EINTR) {
            continue;
        } else {
            std::fprintf(stderr,
                         "[%s] removing client from being served - read error (errno: %d, retval: '%s')!\n",
                         to_string().c_str(),
                         status.error(),
                         strerror(status.error()));
            break;
        }
    }

    co_return retval;
}

deferred<bool> coroutine_session::write(const std::string& outline)
{
    const char* line = outline.c_str();
    std::size_t len = outline.size();

    do {
        iostatus status = co_await schedule_write(line, len);

        if (status.has_value()) {
            if (status.value() > 0) {
                //std::fprintf(stderr, "Send %d bytes\n", status.value());
                if (static_cast<decltype(len)>(status.value()) > len) {
                    status = std::unexpected{EOVERFLOW};
                    break; /* paranoid android */
                }
                len -= status.value();
                line += status.value();
            }
        } else if (status.error() == EINTR) {
            continue;
        } else {
            std::fprintf(stderr,
                         "[%s] removing client from being served - write error (errno: %d, retval: '%s')!\n",
                         to_string().c_str(),
                         status.error(),
                         strerror(status.error()));
            break;
        }
    } while (len > 0);

    co_return (len == 0);
}

deferred<bool> coroutine_session::worker()
{
    bool status;
    charbuffer buffer(4096);

    for (;;) {
        status = co_await read(buffer);
        if (!status)
            co_return status;

        const char* line;
        do {
            std::size_t len;
            line = buffer.getline(&len);
            if (line) {
                //fprintf(stdout, "%s\n", line);
                std::string outline = "echo: " + std::string(line, len) + "\n";
                status = co_await write(outline);
                if (!status)
                    co_return status;
            }
        } while (line != nullptr);
    }

    co_return true;
}

job coroutine_session::setup_termination_handler(std::function<void()> completion)
{
    char buf[4];
    iostatus status;

    do {
        status = co_await schedule(m_iouring.read(m_pipefds[PIPE_READ_END], buf, sizeof(buf)));
    } while (!status.has_value() && status.error() == EINTR);

    completion();
}

iostatus coroutine_session::io_run()
{
    iostatus retval = std::unexpected{EFAULT};
    io_uring_cqe* cqe;

    for (;;) {
        if (m_stop_requested) {
            retval = 0;
            break;
        }

        int status = m_iouring.wait_cqe(&cqe);
        if (status < 0) {
            if (status == -EINTR) // system call was interrupted
                continue;
            std::fprintf(stderr, "m_iouring.wait_cqe() failed with code %d (%s)\n", -status, errnotostr(-status));
            retval = std::unexpected{-status};
            break;
        }

        m_iouring.consume(1);
        if (cqe->user_data == 0) {
            std::fprintf(stderr, "empty user data in completion entry - bailing out\n");
            retval = std::unexpected{EIO};
            break;
        }

        user_data* ud = reinterpret_cast<user_data*>(cqe->user_data);
        asiohandle<iostatus>* asiohndl = ud->asiohndl;
        if (asiohndl != nullptr)
            asiohndl->done(cqe->res < 0 ? std::unexpected{-cqe->res} : iostatus{cqe->res});
    }

    return retval;
}

void coroutine_session::thread_function()
{
    std::fprintf(stdout, "[%s] session thread initialized\n", to_string().c_str());

    setup_termination_handler([this] {
        std::fprintf(stderr, "[%s] received termination request\n", to_string().c_str());
        m_stop_requested = true;
    });

    launch(worker(), [this] {
        std::fprintf(stderr, "[%s] worker coroutine thread terminated\n", to_string().c_str());
        m_stop_requested = true;
    });

    iostatus status = io_run(); // This call blocks until client closes the connection
                                // or scheduler gets interrupted

    std::fprintf(stdout, "[%s] session thread terminated with %s\n",
        to_string().c_str(), status.has_value() ? "success" : "failure");

    m_args.release(shared_from_this());
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
