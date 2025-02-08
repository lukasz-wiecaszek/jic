/* SPDX-License-Identifier: MIT */
/**
 * @file tcpserver.cpp
 *
 * tcpserver example
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * TODO:
 * - replace ::poll() by epoll facility
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdio>
#include <cassert>
#include <cstring>
#include <algorithm>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "errnotostr.hpp"
#include "utilities.hpp"
#include "oldschool_session.hpp"
#include "iouring_session.hpp"
#include "coroutine_session.hpp"

#include "tcpserver.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace lts;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define TCPSERVER_POLL_TIMEOUT_MS (100 * 1000)
#define TCPSERVER_LISTEN_BACKLOG  16

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
tcpserver::tcpserver(const std::string& address, int port, int max_sessions)
: m_address{address}
, m_port{port}
, m_max_sessions{max_sessions}
, m_sockfd{INVALID_FD}
, m_pipefds{INVALID_FD, INVALID_FD}
, m_sessions{}
, m_sessions_mutex{}
, m_condvar{}
, m_thread{}
, m_future{}
{
    std::fprintf(stdout, "[%s] tcpserver created\n", to_string().c_str());
}

tcpserver::~tcpserver()
{
    stop();
    close_sockfd(m_sockfd);
    close_fd(m_pipefds[PIPE_READ_END]);
    close_fd(m_pipefds[PIPE_WRIRE_END]);

    std::fprintf(stdout, "[%s] tcpserver destroyed\n", to_string().c_str());
}

bool tcpserver::start()
{
    if (m_thread != nullptr)
        return false;

    std::promise<iostatus> promise;
    m_future = promise.get_future();
    m_thread = std::make_unique<std::jthread>(&tcpserver::thread_function, this, std::move(promise));

    return true;
}

void tcpserver::stop()
{
    if (m_thread) {
        // wait until all sessions terminate
        std::unique_lock<std::mutex> lock(m_sessions_mutex);
        std::fprintf(stdout, "[%s] stopping server when %zu sessions active\n", to_string().c_str(), m_sessions.size());
        std::for_each(m_sessions.cbegin(), m_sessions.cend(), [](std::shared_ptr<session> session) {
            session->terminate();
        });
        m_condvar.wait(lock, [this]() {
            return m_sessions.size() == 0U;
        });

        // write any character to pipe - this will wake poll on the pipe's reading end
        write_full(m_pipefds[PIPE_WRIRE_END], "!", 1);
    }
}

iostatus tcpserver::join()
{
    return m_thread->join(), m_thread = nullptr, m_future.get();
}

/*===========================================================================*\
 * class protected functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class private functions definitions
\*===========================================================================*/
iostatus tcpserver::init()
{
    int status;

    status = ::pipe(m_pipefds);
    assert("pipe()" && status >= -1);
    if (status == -1) {
        std::fprintf(stderr, "[%s] pipe() failed with code %d (%s)\n", to_string().c_str(), errno, errnotostr(errno));
        return std::unexpected{errno};
    }

    status = set_nonblocking_flag(m_pipefds[PIPE_READ_END]);
    if (status != 0) {
        std::fprintf(stderr, "[%s] could not set pipe's reading end to non-blocking mode\n", to_string().c_str());
        return std::unexpected{status};
    }

    m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert("socket()" && ((m_sockfd == -1 /* error, errno is set */) || (m_sockfd >= 0 /* success */)));
    if (m_sockfd == INVALID_FD) {
        std::fprintf(stderr,
                     "[%s] socket(AF_INET, SOCK_STREAM, 0) failed with code %d (%s)\n",
                     to_string().c_str(),
                     errno,
                     errnotostr(errno));
        return std::unexpected{errno};
    }

    int enable_reuse_addr = 1;
    status = ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable_reuse_addr, sizeof(enable_reuse_addr));
    if (status == -1) {
        std::fprintf(stderr,
                     "[%s] setsockopt(SOL_SOCKET, SO_REUSEADDR) failed with code %d (%s)\n",
                     to_string().c_str(),
                     errno,
                     errnotostr(errno));
    }

    struct sockaddr_in bind_sockaddr;
    std::memset(&bind_sockaddr, 0, sizeof(bind_sockaddr));

    bind_sockaddr.sin_family = AF_INET;
    bind_sockaddr.sin_port = htons(m_port);
    status = ::inet_pton(AF_INET, m_address.c_str(), &(bind_sockaddr.sin_addr));
    assert("inet_pton()" &&
           ((status == -1 /* error, errno is set */) || (status == 0 /* still error */) || (status == 1 /* success */)));
    if ((status == -1) || (status == 0)) {
        std::fprintf(stderr, "[%s] inet_pton() failed\n", to_string().c_str());
        return status == 0 ? std::unexpected{EFAULT} : std::unexpected{errno};
    }

    status = ::bind(m_sockfd, reinterpret_cast<struct sockaddr*>(&bind_sockaddr), sizeof(bind_sockaddr));
    assert("bind()" && status >= -1);
    if (status == -1) {
        std::fprintf(stderr,
                     "[%s] bind(%s:%d) failed with code %d (%s)\n",
                     to_string().c_str(),
                     m_address.c_str(),
                     m_port,
                     errno,
                     errnotostr(errno));
        return std::unexpected{errno};
    }

    status = ::listen(m_sockfd, TCPSERVER_LISTEN_BACKLOG);
    assert("listen()" && status >= -1);
    if (status == -1) {
        std::fprintf(stderr,
                     "[%s] listen(%d) failed with code %d (%s)\n",
                     to_string().c_str(),
                     TCPSERVER_LISTEN_BACKLOG,
                     errno,
                     errnotostr(errno));
        return std::unexpected{errno};
    }

    return 0;
}

void tcpserver::session_create()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sockfd = accept(m_sockfd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);

    if (client_sockfd >= 0) {
        std::fprintf(stdout,
                     "[%s] accepted new client from '%s:%u'\n",
                     to_string().c_str(),
                     inet_ntoa(client_addr.sin_addr),
                     ntohs(client_addr.sin_port));

        session_args args;
        args.sockfd = client_sockfd;
        args.addr = client_addr;
        args.release = [this](std::shared_ptr<session> session) {
            session_destroy(session);
        };

        //std::shared_ptr<oldschool_session> session = std::make_shared<oldschool_session>(args);
        //std::shared_ptr<iouring_session> session = std::make_shared<iouring_session>(args);
        std::shared_ptr<coroutine_session> session = std::make_shared<coroutine_session>(args);
        if (session) {
            std::lock_guard<std::mutex> lock(m_sessions_mutex);
            m_sessions.push_back(session);
        }
    }
}

void tcpserver::session_destroy(std::shared_ptr<session> session)
{
    {
        std::lock_guard<std::mutex> lock(m_sessions_mutex);
        std::fprintf(stdout, "[%s] session_destroy: number of active sessions: %zu\n",
            to_string().c_str(), m_sessions.size());
        m_sessions.remove(session);
    }
    m_condvar.notify_one();
}

iostatus tcpserver::worker()
{
    int poll_res;
    struct pollfd poll_fds[] = {
        {m_pipefds[PIPE_READ_END], POLLIN, 0},
        {                m_sockfd, POLLIN, 0}
    };
    iostatus status = std::unexpected{EFAULT};

    for (;;) {
        poll_res = ::poll(poll_fds, std::size(poll_fds), TCPSERVER_POLL_TIMEOUT_MS);

        if (poll_res < 0) {
            std::fprintf(stderr, "[%s] poll() failed with code %d (%s)\n", to_string().c_str(), errno, errnotostr(errno));
            status = std::unexpected{errno};
            break;
        } else if (0 == poll_res) {
            continue;
        } else {
            if (0 != (POLLIN & poll_fds[0].revents)) {
                std::fprintf(stderr, "[%s] received termination request\n", to_string().c_str());
                flush_pipe(m_pipefds[PIPE_READ_END]);
                status = 0;
                break;
            }
            if (0 != (POLLIN & poll_fds[1].revents)) {
                {
                    std::unique_lock<std::mutex> lock(m_sessions_mutex);
                    if (m_sessions.size() >= static_cast<std::size_t>(m_max_sessions)) {
                        std::fprintf(stdout,
                                     "[%s] cannot handle more then %d sessions, please wait...\n",
                                     to_string().c_str(),
                                     m_max_sessions);
                        m_condvar.wait(lock);
                        continue;
                    }
                }

                session_create();
            }
        }
    }

    return status;
}

void tcpserver::thread_function(std::promise<iostatus>&& promise)
{
    std::fprintf(stdout, "[%s] server thread (pid: %d, tid: %d) initialized\n", to_string().c_str(), getpid(), gettid());

    do {
        iostatus status;

        status = init();
        if (!status.has_value()) {
            promise.set_value(status);
            break;
        }

        promise.set_value(worker());
    } while (0);

    std::fprintf(stdout, "[%s] server thread (pid: %d, tid: %d) terminated\n", to_string().c_str(), getpid(), gettid());
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
