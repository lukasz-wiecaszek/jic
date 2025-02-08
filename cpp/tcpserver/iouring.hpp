/**
 * @file iouring.hpp
 *
 * A wrapper on the liburing with networking extensions.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _IOURING_HPP_
#define _IOURING_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <liburing.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

class iouring {
public:
    iouring(unsigned sq_entries);
    ~iouring();

    // iouring shall be non-copyable and non-movable
    iouring(const iouring&) = delete;
    iouring(iouring&&) = delete;
    iouring& operator=(const iouring&) = delete;
    iouring& operator=(iouring&&) = delete;

    bool is_valid() const { return m_state == state_e::initialized; }
    operator bool() const { return is_valid(); }

    int submit();
    int submit_and_wait(unsigned int wait_nr = 1);
    unsigned int peek(io_uring_cqe** cqes, std::size_t count);

    template<std::size_t N>
    unsigned int peek(io_uring_cqe* (&cqes)[N])
    {
        return peek(cqes, N);
    }

    int wait_cqe(io_uring_cqe** cqe);
    void consume(unsigned int nr);

    io_uring_sqe* read(int fd, void* buf, std::size_t count, off_t offset = 0l);
    io_uring_sqe* write(int fd, const void* buf, std::size_t count, off_t offset = 0l);

private:
    io_uring_sqe* get_sqe(uint8_t opcode, int fd, uint64_t off, const void* addr, uint32_t len);

private:
    enum class state_e {
        uninitialized,
        initialized
    } m_state;
    io_uring m_ring;
    io_uring_params m_params;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace lts
{
} /* end of namespace lts */

#endif /* _IOURING_HPP_ */
