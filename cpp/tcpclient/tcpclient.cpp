/* SPDX-License-Identifier: MIT */
/**
 * @file tcpclient.cpp
 *
 * tcpclient example
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdio>
#include <cassert>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "utilities.hpp"
#include "tcpclient.hpp"

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
tcpclient::tcpclient(const std::string& name, const std::string& address, int port)
: m_name{name}
, m_address{address}
, m_port{port}
, m_thread{}
, m_future{}
{
    std::fprintf(stdout, "[%s] tcpclient created\n", to_string().c_str());
}

tcpclient::~tcpclient()
{
    std::fprintf(stdout, "[%s] tcpclient destroyed\n", to_string().c_str());
}

bool tcpclient::start()
{
    if (m_thread != nullptr)
        return false;

    std::promise<iostatus> promise;
    m_future = promise.get_future();
    m_thread = std::make_unique<std::jthread>(&tcpclient::thread_function, this, std::move(promise));

    return true;
}

void tcpclient::stop()
{
    if (m_thread) {
        std::stop_token stoken = m_thread->get_stop_token();
        if (stoken.stop_possible() && !stoken.stop_requested())
            m_thread->request_stop();
    }
}

iostatus tcpclient::join()
{
    return m_thread->join(), m_thread = nullptr, m_future.get();
}

/*===========================================================================*\
 * class protected functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class private functions definitions
\*===========================================================================*/
iostatus tcpclient::init()
{
    int status;
    int sockfd;

    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert("socket()" && ((sockfd == -1 /* error, errno is set */) || (sockfd >= 0 /* success */)));
    if (sockfd == INVALID_FD) {
        std::fprintf(stderr,
                     "[%s] socket(AF_INET, SOCK_STREAM, 0) failed with code %d (%s)\n",
                     to_string().c_str(),
                     errno,
                     errnotostr(errno));
        return std::unexpected{errno};
    }

    struct sockaddr_in server_sockaddr;
    std::memset(&server_sockaddr, 0, sizeof(server_sockaddr));

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(m_port);
    status = ::inet_pton(AF_INET, m_address.c_str(), &(server_sockaddr.sin_addr));
    assert("inet_pton()" &&
           ((status == -1 /* error, errno is set */) || (status == 0 /* still error */) || (status == 1 /* success */)));
    if ((status == -1) || (status == 0)) {
        std::fprintf(stderr, "[%s] inet_pton() failed\n", to_string().c_str());
        return status == 0 ? std::unexpected{EFAULT} : std::unexpected{errno};
    }

    status = ::connect(sockfd, reinterpret_cast<struct sockaddr*>(&server_sockaddr), sizeof(server_sockaddr));
    assert("connect()" && ((status == -1 /* error, errno is set */) || (status == 0 /* success */)));
    if (status == -1) {
        std::fprintf(stderr,
                     "[%s] connect(%s:%d) failed with code %d (%s)\n",
                     to_string().c_str(),
                     m_address.c_str(),
                     m_port,
                     errno,
                     errnotostr(errno));
        return std::unexpected{errno};
    }

    return sockfd;
}

iostatus tcpclient::write(int sockfd, const std::string& text)
{
    const char* line = text.c_str();
    std::size_t len = text.size();
    do {
        ssize_t status = ::write(sockfd, line, len);
        if (status > 0) {
            if (static_cast<decltype(len)>(status) > len)
                return std::unexpected{E2BIG}; /* paranoid android */
            len -= status;
            line += status;
        } else if (status < 0) {
            if (errno == EINTR)
                continue;
            else {
                std::fprintf(stderr,
                             "[%s] terminating client - write() failed with code %d (%s)\n",
                             to_string().c_str(),
                             errno,
                             strerror(errno));
                return std::unexpected{errno};
            }
        } else {
            /* do nothing */
        }
    } while (len > 0);

    return 0;
}

iostatus tcpclient::read(int sockfd, charbuffer& buffer)
{
    for (;;) {
        buffer.move();
        ssize_t status = ::read(sockfd, buffer.write_ptr(), buffer.write_available());
        if (status > 0) {
            buffer.produce(status);
            break;
        } else if (status < 0) {
            if (errno == EINTR)
                continue;
            else {
                std::fprintf(stderr,
                             "[%s] terminating client - read() failed with code %d (%s)\n",
                             to_string().c_str(),
                             errno,
                             strerror(errno));
                return std::unexpected{errno};
            }
        } else {
            std::fprintf(stderr, "[%s] terminating client - connection closed!\n", to_string().c_str());
            return std::unexpected{0};
        }
    }

    return 0;
}

iostatus tcpclient::worker(int sockfd)
{
    iostatus status;
    charbuffer buffer(4096);
    std::stop_token stoken = m_thread->get_stop_token();
    int send_receive_cnt = 0;
    int line_cnt = 0;

    const std::string first_witch = "When shall we three meet again — In thunder, lightning, or in rain?";
    const std::string second_witch = "When the hurlyburly's done, when the battle's lost, and won.";
    const std::string third_witch = "That will be ere the set of sun.";
    const std::string text = first_witch + "\n" + second_witch + "\n" + third_witch + "\n";
    const std::string prefix = "echo: ";

    while (!stoken.stop_requested()) {
        status = write(sockfd, text);
        if (!status.has_value())
            return status;

        //std::this_thread::sleep_for(std::chrono::seconds(1));

        status = read(sockfd, buffer);
        if (!status.has_value())
            return status;

        const char* line;
        do {
            std::size_t len;
            line = buffer.getline(&len);
            if (line) {
                //fprintf(stdout, "%s\n", line);
                if (std::memcmp(line, prefix.c_str(), prefix.size()) != 0)
                    return std::unexpected{EBADMSG};
                switch (line_cnt % 3) {
                    case 0:
                    {
                        if (std::memcmp(line + prefix.size(), first_witch.c_str(), first_witch.size()) != 0)
                            return std::unexpected{EBADMSG};
                    } break;
                    case 1:
                    {
                        if (std::memcmp(line + prefix.size(), second_witch.c_str(), second_witch.size()) != 0)
                            return std::unexpected{EBADMSG};
                    } break;
                    case 2:
                    {
                        if (std::memcmp(line + prefix.size(), third_witch.c_str(), third_witch.size()) != 0)
                            return std::unexpected{EBADMSG};
                    } break;
                }
                line_cnt++;
            }
        } while (line != nullptr);

        send_receive_cnt++;
    }

    return send_receive_cnt;
}

void tcpclient::thread_function(std::promise<iostatus>&& promise)
{
    int sockfd = INVALID_FD;

    std::fprintf(stdout, "[%s] client thread (pid: %d, tid: %d) initialized\n", to_string().c_str(), getpid(), gettid());

    do {
        iostatus status;

        status = init();
        if (!status.has_value()) {
            promise.set_value(status);
            break;
        }

        sockfd = status.value();
        status = worker(sockfd);

        promise.set_value(status);
    } while (0);

    close_sockfd(sockfd);

    std::fprintf(stdout, "[%s] client thread (pid: %d, tid: %d) terminated\n", to_string().c_str(), getpid(), gettid());
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
