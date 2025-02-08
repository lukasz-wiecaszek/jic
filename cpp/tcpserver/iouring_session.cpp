/* SPDX-License-Identifier: MIT */
/**
 * @file iouring_session.cpp
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

#include "iouring_session.hpp"

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
iouring_session::iouring_session(const session_args& args)
: session{args}
, std::enable_shared_from_this<iouring_session>{}
, m_iouring{NUM_SQ_ENTRIES}
, m_user_data{}
, m_free_slots{}
, m_thread{}
, m_pipefds{INVALID_FD, INVALID_FD}
{
    do {
        int retval;

        if (!m_iouring.is_valid())
            break;

        for (int i = 0; i < NUM_SQ_ENTRIES; i++) {
            m_user_data[i].index = i;
            m_user_data[i].iohndl = nullptr;
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

        m_thread = std::thread(&iouring_session::thread_function, this);
        m_thread.detach();

        std::fprintf(stdout, "[%s] iouring_session created and initialized\n", to_string().c_str());
        m_state = state_e::initialized;
    } while (0);
}

iouring_session::~iouring_session()
{
    close_fd(m_pipefds[PIPE_READ_END]);
    close_fd(m_pipefds[PIPE_WRIRE_END]);

    std::fprintf(stdout, "[%s] iouring_session destroyed\n", to_string().c_str());
}

void iouring_session::terminate()
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
iostatus iouring_session::schedule(io_uring_sqe* sqe, std::function<bool(iostatus status)> callback)
{
    if (sqe == nullptr) {
        std::fprintf(stderr, "sqe == nullptr\n");
        return std::unexpected{EFAULT};
    }

    user_data* ud = get_user_data();
    if (ud == nullptr) {
        std::fprintf(stderr, "ud == nullptr\n");
        return std::unexpected{ENODEV};
    }

    sqe->user_data = reinterpret_cast<uint64_t>(ud);

    int status = m_iouring.submit();
    if (status <= 0) {
        std::fprintf(stderr, "m_iouring.submit() failed\n");
        return std::unexpected{-status};
    }

    ud->iohndl = std::make_unique<iohandle<iostatus>>([this, ud, callback](iostatus status) {
        bool retval = callback(status);
        put_user_data(ud);
        return retval;
    });

    return 0;
}

iostatus iouring_session::schedule_read(charbuffer& buffer)
{
    buffer.move();

    io_uring_sqe* sqe = m_iouring.read(m_args.sockfd, buffer.write_ptr(), buffer.write_available());

    return schedule(sqe, [this, &buffer](iostatus status) {
        if (handle_read(status, buffer) == false)
            return false;

        const char* line;
        do {
            std::size_t len;
            line = buffer.getline(&len);
            if (line) {
                //fprintf(stdout, "%s\n", line);
                std::string outline = "echo: " + std::string(line, len) + "\n";
                if (schedule_write(outline) == false)
                    return false;
            }
        } while (line != nullptr);

        // we may now schedule next read
        status = schedule_read(buffer);
        if (!status)
            return false;

        return true;
    });
}

iostatus iouring_session::schedule_write(const std::string& outline)
{
    io_uring_sqe* sqe = m_iouring.write(m_args.sockfd, outline.c_str(), outline.size());

    return schedule(sqe, [this, outline](iostatus status) {
        return handle_write(status, outline);
    });
}

bool iouring_session::handle_read(iostatus status, charbuffer& buffer)
{
    bool retval = false;

    do {
        if (status.has_value()) {
            if (status.value() > 0) {
                //std::fprintf(stderr, "Received %d bytes\n", status.value());
                buffer.produce(status.value());
            } else {
                std::fprintf(stderr, "[%s] removing client from being served - connection closed!\n", to_string().c_str());
                break;
            }
        } else if (status.error() == EINTR) {
            if (schedule_read(buffer) == false) {
                break;
            }
        } else {
            std::fprintf(stderr,
                         "[%s] removing client from being served - read error (errno: %d, retval: '%s')!\n",
                         to_string().c_str(),
                         status.error(),
                         strerror(status.error()));
            break;
        }

        retval = true;
    } while (0);

    return retval;
}

bool iouring_session::handle_write(iostatus status, std::string outline)
{
    bool retval = false;

    do {
        if (status.has_value()) {
            //std::fprintf(stderr, "Sent %d bytes\n", status.value());
            if (status.value() > 0) {
                if (static_cast<decltype(outline.size())>(status.value()) > outline.size())
                    break; /* paranoid android */
                outline.resize(outline.size() - status.value());
                if (!outline.empty())
                    if (schedule_write(outline) == false)
                        break;
            }
        } else if (status.error() == EINTR) {
            if (schedule_write(outline) == false)
                break;
        } else {
            std::fprintf(stderr,
                         "[%s] removing client from being served - write error (errno: %d, retval: '%s')!\n",
                         to_string().c_str(),
                         status.error(),
                         strerror(status.error()));
            break;
        }

        retval = true;
    } while (0);

    return retval;
}

iostatus iouring_session::setup_termination_handler()
{
    char buf[4];

    io_uring_sqe* sqe = m_iouring.read(m_pipefds[PIPE_READ_END], buf, sizeof(buf));

    return schedule(sqe, [this](iostatus status) {
        std::fprintf(stderr, "[%s] received termination request\n", to_string().c_str());
        return false;
    });
}

iostatus iouring_session::io_run()
{
    iostatus retval = std::unexpected{EFAULT};
    io_uring_cqe* cqe;
    charbuffer buffer(4096);

    schedule_read(buffer);

    for (;;) {
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
        std::unique_ptr<iohandle<iostatus>>& iohndl = ud->iohndl;
        if (iohndl != nullptr) {
            bool done = iohndl->done(cqe->res < 0 ? std::unexpected{-cqe->res} : iostatus{cqe->res});
            if (!done) {
                retval = 0;
                break;
            }
        }
    }

    return retval;
}

void iouring_session::thread_function()
{
    iostatus status = std::unexpected{EFAULT};

    std::fprintf(stdout, "[%s] session thread initialized\n", to_string().c_str());

    do {
        status = setup_termination_handler();
        if (!status)
            break;

        status = io_run(); // This call blocks until client closes the connection
                           // or scheduler gets interrupted
    } while (0);

    std::fprintf(stdout, "[%s] session thread terminated with %s\n",
        to_string().c_str(), status.has_value() ? "success" : "failure");

    m_args.release(shared_from_this());
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
