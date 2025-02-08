/* SPDX-License-Identifier: MIT */
/**
 * @file iouring.cpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstring>
#include <cstdio>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "errnotostr.hpp"

#include "iouring.hpp"

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
iouring::iouring(unsigned sq_entries)
: m_state{state_e::uninitialized}
, m_ring{}
, m_params{}
{
    do {
        int status;

        std::memset(&m_params, 0, sizeof(m_params));

        status = io_uring_queue_init_params(sq_entries, &m_ring, &m_params);
        if (status < 0) {
            //io_uring_queue_init_params returns 0 on success and -errno on failure
            std::fprintf(stderr, "pipe() failed with code %d (%s)\n", -status, errnotostr(-status));
            break;
        }

        m_state = state_e::initialized;
    } while (0);
}

iouring::~iouring()
{
    if (m_state == state_e::initialized)
        io_uring_queue_exit(&m_ring);
}

int iouring::submit()
{
    return io_uring_submit(&m_ring);
}

int iouring::submit_and_wait(unsigned int wait_nr)
{
    return io_uring_submit_and_wait(&m_ring, wait_nr);
}

unsigned int iouring::peek(io_uring_cqe** cqes, std::size_t count)
{
    return io_uring_peek_batch_cqe(&m_ring, cqes, count);
}

int iouring::wait_cqe(io_uring_cqe** cqe)
{
    return io_uring_wait_cqe(&m_ring, cqe);
}

void iouring::consume(unsigned int nr)
{
    io_uring_cq_advance(&m_ring, nr);
}

io_uring_sqe* iouring::read(int fd, void* buf, std::size_t count, off_t offset)
{
    return get_sqe(IORING_OP_READ, fd, offset, buf, count);
}

io_uring_sqe* iouring::write(int fd, const void* buf, std::size_t count, off_t offset)
{
    return get_sqe(IORING_OP_WRITE, fd, offset, buf, count);
}

/*===========================================================================*\
 * class protected functions definitions
\*===========================================================================*/

/*===========================================================================*\
 * class private functions definitions
\*===========================================================================*/
io_uring_sqe* iouring::get_sqe(uint8_t opcode, int fd, uint64_t off, const void* addr, uint32_t len)
{
    io_uring_sqe* sqe = nullptr;

    do {
        sqe = io_uring_get_sqe(&m_ring);
        if (sqe == nullptr)
            break;

        std::memset(sqe, 0, sizeof(*sqe));
        sqe->opcode = opcode;
        sqe->fd = fd;
        sqe->off = off;
        sqe->addr = reinterpret_cast<uint64_t>(addr);
        sqe->len = len;

    } while (0);

    return sqe;
}

/*===========================================================================*\
 * global (external linkage) functions definitions
\*===========================================================================*/
