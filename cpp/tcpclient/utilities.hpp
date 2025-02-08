/* SPDX-License-Identifier: MIT */
/**
 * @file utilities.hpp
 *
 * Set of general purpose utility routines.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _UTILITIES_HPP_
#define _UTILITIES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/socket.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "errnotostr.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PIPE_READ_END  0
#define PIPE_WRIRE_END 1
#define INVALID_FD     (-1)

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

static inline void close_fd(int fd)
{
    if (fd >= 0)
        ::close(fd);
}

static inline void close_sockfd(int sockfd)
{
    if (sockfd >= 0) {
        ::shutdown(sockfd, SHUT_RD);
        ::close(sockfd);
    }
}

static inline void flush_pipe(int fd)
{
    int status;

    do {
        char buf;
        status = ::read(fd, &buf, sizeof(buf));
    } while (status > 0);
}

static inline int set_nonblocking_flag(int fd)
{
    int retval = -EFAULT;

    do {
        int status;
        int flags;

        flags = ::fcntl(fd, F_GETFL);
        assert("fcntl()" && ((status == -1 /* error, errno is set */) || (status >= 0 /* success */)));
        if (flags == -1) {
            retval = errno;
            break;
        }

        flags |= O_NONBLOCK;

        status = ::fcntl(fd, F_SETFL, flags);
        assert("fcntl()" && ((status == -1 /* error, errno is set */) || (status >= 0 /* success */)));
        if (status == -1) {
            retval = errno;
            break;
        }

        retval = 0; /* success */
    } while (0);

    return retval;
}

static inline int clear_nonblocking_flag(int fd)
{
    int retval = -EFAULT;

    do {
        int status;
        int flags;

        flags = ::fcntl(fd, F_GETFL);
        assert("fcntl()" && ((status == -1 /* error, errno is set */) || (status >= 0 /* success */)));
        if (flags == -1) {
            retval = errno;
            break;
        }

        flags &= ~O_NONBLOCK;

        status = ::fcntl(fd, F_SETFL, flags);
        assert("fcntl()" && ((status == -1 /* error, errno is set */) || (status >= 0 /* success */)));
        if (status == -1) {
            retval = errno;
            break;
        }

        retval = 0; /* success */
    } while (0);

    return retval;
}

static inline bool read_full(int fd, char* buffer, std::size_t size)
{
    bool retval = true;
    std::size_t cnt = 0;

    do {
        ssize_t res = ::read(fd, buffer + cnt, size - cnt);
        if (res < 0) {
            retval = false;
            break;
        }
        cnt += res;
    } while (cnt < size);

    return retval;
}

static inline bool write_full(int fd, const char* buffer, std::size_t size)
{
    bool retval = true;
    std::size_t written = 0;

    do {
        ssize_t res = ::write(fd, buffer + written, size - written);
        if (res < 0) {
            retval = false;
            break;
        }
        written += res;
    } while (written < size);

    return retval;
}

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

#endif /* _UTILITIES_HPP_ */
