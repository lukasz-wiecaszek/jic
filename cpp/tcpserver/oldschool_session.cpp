/* SPDX-License-Identifier: MIT */
/**
 * @file oldschool_session.cpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdio>
#include <cassert>
#include <chrono>

#include <poll.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "errnotostr.hpp"
#include "utilities.hpp"

#include "oldschool_session.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace lts;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define SESSION_POLL_TIMEOUT_MS (100 * 1000)

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
oldschool_session::oldschool_session(const session_args& args)
: session{args}
, std::enable_shared_from_this<oldschool_session>{}
, m_thread{}
, m_pipefds{INVALID_FD, INVALID_FD}
{
    do {
        int retval;

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

        m_thread = std::thread(&oldschool_session::thread_function, this);
        m_thread.detach();

        std::fprintf(stdout, "[%s] oldschool_session created and initialized\n", to_string().c_str());
        m_state = state_e::initialized;
    } while (0);
}

oldschool_session::~oldschool_session()
{
    close_fd(m_pipefds[PIPE_READ_END]);
    close_fd(m_pipefds[PIPE_WRIRE_END]);

    std::fprintf(stdout, "[%s] oldschool_session destroyed\n", to_string().c_str());
}

void oldschool_session::terminate()
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
bool oldschool_session::read(int sockfd, charbuffer& buffer)
{
    bool retval = false;

    for (;;) {
        buffer.move();
        ssize_t status = ::read(sockfd, buffer.write_ptr(), buffer.write_available());
        if (status > 0) {
            //std::fprintf(stderr, "Received %ld bytes\n", status);
            buffer.produce(status);
            retval = true;
            break;
        } else if (status < 0) {
            if (errno == EINTR)
                continue;
            else {
                std::fprintf(stderr,
                             "[%s] removing client from being served - read error (errno: %d, retval: '%s')!\n",
                             to_string().c_str(),
                             errno,
                             strerror(errno));
                break;
            }
        } else {
            std::fprintf(stderr, "[%s] removing client from being served - connection closed!\n", to_string().c_str());
            break;
        }
    }

    return retval;
}

bool oldschool_session::write(int sockfd, const std::string& outline)
{
    const char* line = outline.c_str();
    std::size_t len = outline.size();

    do {
        ssize_t status = ::write(sockfd, line, len);
        //std::fprintf(stderr, "Sent %ld bytes\n", status);
        if (status > 0) {
            if (static_cast<decltype(len)>(status) > len)
                break; /* paranoid android */
            len -= status;
            line += status;
        } else if (status < 0) {
            if (errno == EINTR)
                continue;
            else {
                std::fprintf(stderr,
                             "[%s] removing client from being served - write error (errno: %d, retval: '%s')!\n",
                             to_string().c_str(),
                             errno,
                             strerror(errno));
                break;
            }
        } else {
            /* do nothing */
        }
    } while (len > 0);

    return (len == 0);
}

bool oldschool_session::handler(int sockfd, charbuffer& buffer)
{
    bool retval = false;

    do {
        bool status;

        status = read(sockfd, buffer);
        if (status == false)
            break;

        const char* line;
        do {
            std::size_t len;
            line = buffer.getline(&len);
            if (line) {
                //fprintf(stdout, "%s\n", line);
                std::string outline = "echo: " + std::string(line, len) + "\n";
                status = write(sockfd, outline);
                if (status == false)
                    break;
            }
        } while (line != nullptr);

        if (status == false)
            break;

        retval = true;
    } while (0);

    return retval;
}

void oldschool_session::thread_function()
{
    charbuffer buffer(4096);

    std::fprintf(stdout, "[%s] session thread initialized\n", to_string().c_str());

    int poll_res;
    struct pollfd poll_fds[2] = {
        {m_pipefds[PIPE_READ_END], POLLIN, 0},
        {           m_args.sockfd, POLLIN, 0}
    };

    for (;;) {
        poll_res = ::poll(poll_fds, std::size(poll_fds), SESSION_POLL_TIMEOUT_MS);

        if (poll_res < 0) {
            std::fprintf(stderr, "[%s] poll() failed with code %d (%s)\n", to_string().c_str(), errno, errnotostr(errno));
            break;
        } else if (poll_res == 0) {
            std::fprintf(stderr, "[%s] poll() timeout\n", to_string().c_str());
            continue;
        } else {
            if (0 != (POLLIN & poll_fds[0].revents)) {
                std::fprintf(stderr, "[%s] received termination request\n", to_string().c_str());
                flush_pipe(m_pipefds[PIPE_READ_END]);
                break;
            }
            if (0 != (POLLIN & poll_fds[1].revents)) {
                if (handler(m_args.sockfd, buffer) == false)
                    break;
            }
        }
    }

    std::fprintf(stdout, "[%s] session thread terminated\n", to_string().c_str());
    m_args.release(shared_from_this());
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
