/**
 * @file ringbuffer.hpp
 *
 * Definition of generic ringbuffer design pattern.
 * Its non-blocking flavour is completely lockless.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef _RINGBUFFER_HPP_
#define _RINGBUFFER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <atomic>
#include <string>
#include <sstream>
#include <functional>

#include <cstring>
#include <climits>

#if defined(DEBUG_RINGBUFFER)
#include <iostream>
#endif

#if !defined(CACHELINE_SIZE)
#define CACHELINE_SIZE 64
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "../../utils/power_of_two.hpp"
#include "../../utils/utilities.hpp"
#include "../../semaphores/binary/binary_semaphore.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

enum class ringbuffer_status
{
    OK = 0,
    INTERNAL_ERROR = -1,
    WOULD_BLOCK = -2,
};

enum class ringbuffer_role
{
    NONE,
    PRODUCER,
    CONSUMER,
};

enum class ringbuffer_xfer_semantic
{
    COPY,
    MOVE,
};

template<typename T, std::size_t CAPACITY, bool NON_BLOCKING = false>
class ringbuffer
{
public:
    typedef T value_type;

    explicit ringbuffer() :
        m_counters{},
        m_buffer{nullptr},
        m_writing_semaphore{true},
        m_reading_semaphore{false}
    {
        static_assert(CAPACITY > 0, "ringbuffer's capacity must be greater then 0!");
        static_assert(CAPACITY < LONG_MAX, "ringbuffer's capacity must be lower then LONG_MAX!");

        m_buffer = new T[CAPACITY];

#if defined(DEBUG_RINGBUFFER)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    ~ringbuffer()
    {
#if defined(DEBUG_RINGBUFFER)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

        delete [] m_buffer;
    }

    ringbuffer(const ringbuffer&) = delete;
    ringbuffer(ringbuffer&&) = delete;
    ringbuffer& operator = (const ringbuffer&) = delete;
    ringbuffer& operator = (ringbuffer&&) = delete;

    std::size_t capacity() const
    {
        return CAPACITY;
    }

    bool non_blocking() const
    {
        return NON_BLOCKING;
    }

    ringbuffer_status get_counters(std::size_t* produced, std::size_t* consumed, std::size_t* dropped) const
    {
        std::size_t l_produced = m_counters.m_produced.load(std::memory_order_relaxed);
        std::size_t l_consumed = m_counters.m_consumed.load(std::memory_order_relaxed);

        if (l_produced < l_consumed)
            return ringbuffer_status::INTERNAL_ERROR;

        if ((l_produced - l_consumed) > CAPACITY) /* LONG_MAX is the max capacity */
            return ringbuffer_status::INTERNAL_ERROR;

        if (produced) *produced = l_produced;
        if (consumed) *consumed = l_consumed;

        if (dropped)
            *dropped = m_counters.m_dropped.load(std::memory_order_relaxed);

        return ringbuffer_status::OK;
    }

    void reset(ringbuffer_role role)
    {
        if (ringbuffer_role::PRODUCER == role) {
            std::size_t consumed = m_counters.m_consumed.load(std::memory_order_relaxed);
            m_counters.m_produced.store(consumed, std::memory_order_relaxed);
            m_counters.m_dropped.store(0U, std::memory_order_relaxed);
        }
        else
        if (ringbuffer_role::CONSUMER == role) {
            std::size_t produced = m_counters.m_produced.load(std::memory_order_relaxed);
            m_counters.m_consumed.store(produced, std::memory_order_relaxed);
        }
        else {
            m_counters.reset();
        }
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "ringbuffer@";
        stream << std::hex << this;
        stream << " [capacity: ";
        stream << std::dec << CAPACITY;
        stream << ", ";
        stream << "policy: ";
        if (NON_BLOCKING)
            stream << "non_blocking";
        else
            stream << "blocking";
        stream << " ";
        stream << static_cast<std::string>(m_counters);
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    /*===========================================================================*\
     * write family
    \*===========================================================================*/
    long write(const T* data, std::size_t count)
    {
        return write(data, count, copy<T>);
    }

    long write(T* data, std::size_t count)
    {
        return write(data, count, move<T>);
    }

    long write(const T& data)
    {
        return write(&data, 1, copy<T>);
    }

    long write(T&& data)
    {
        return write(&data, 1, move<T>);
    }

    template<std::size_t N>
    long write(const T (&data)[N])
    {
        return write(data, N, copy<T>);
    }

    template<std::size_t N>
    long write(T (&&data)[N])
    {
        return write(data, N, move<T>);
    }

    long write(std::function<bool(T*)> producer, std::size_t count)
    {
        return write(functor<T>(producer), count, xfer_producer<T>);
    }

    /*===========================================================================*\
     * read family
    \*===========================================================================*/
    long read(T* data, std::size_t count, ringbuffer_xfer_semantic semantic = ringbuffer_xfer_semantic::COPY)
    {
        if (semantic == ringbuffer_xfer_semantic::COPY)
            return read(data, 1, copy<T>);
        else
            return read(data, 1, move<T>);
    }

    long read(T& data, ringbuffer_xfer_semantic semantic = ringbuffer_xfer_semantic::COPY)
    {
        if (semantic == ringbuffer_xfer_semantic::COPY)
            return read(&data, 1, copy<T>);
        else
            return read(&data, 1, move<T>);
    }

    template<std::size_t N>
    long read(T (&data)[N], ringbuffer_xfer_semantic semantic = ringbuffer_xfer_semantic::COPY)
    {
        if (semantic == ringbuffer_xfer_semantic::COPY)
            return read(data, N, copy<T>);
        else
            return read(data, N, move<T>);
    }

    long read(std::function<bool(T*)> consumer, std::size_t count)
    {
        return read(functor<T>(consumer), count, xfer_consumer<T>);
    }

private:
    template<typename U>
    static bool copy(U* dst, const U* src, std::size_t count)
    {
        while (count-- > 0)
            *dst++ = *src++;

        return true;
    }

    template<typename U>
    static bool move(U* dst, U* src, std::size_t count)
    {
        while (count-- > 0)
            *dst++ = std::move(*src++);

        return true;
    }

    template<typename U>
    struct functor
    {
        functor(std::function<bool(U*)> f) :
            m_function{f}
        {
        }

        bool operator()(U* arg)
        {
            return m_function(arg);
        }

        /* This operator does nothing but is needed to fullfil contract
        needed in template write and read functions. */
        void operator += (std::size_t count)
        {
            UNUSED(count);
        }

        std::function<bool(U*)> m_function;
    };

    template<typename U>
    static bool xfer_producer(U* dst, functor<U> src, std::size_t count)
    {
        bool status = true;

        while (count-- > 0)
            if (false == (status = src(dst++)))
                break;

        return status;
    }

    template<typename U>
    static bool xfer_consumer(functor<U> dst, U* src, std::size_t count)
    {
        bool status = true;

        while (count-- > 0)
            if (false == (status = dst(src++)))
                break;

        return status;
    }

    template<typename DST, typename SRC>
    long write(SRC data, std::size_t count, bool (*xfer)(DST, SRC, std::size_t))
    {
        std::size_t produced;
        std::size_t consumed;
        std::size_t free_elements;
        std::size_t write_idx;
        std::size_t split;
        std::size_t remaining;
        ringbuffer_status rbs;

        if (0 == count)
            return 0;

        if (NON_BLOCKING) {
            rbs = get_counters(&produced, &consumed, nullptr);
            if (rbs != ringbuffer_status::OK)
                return static_cast<long>(rbs);

            free_elements = CAPACITY - (produced - consumed);
            if (0 == free_elements) {
                m_counters.m_dropped++;
                return static_cast<long>(ringbuffer_status::WOULD_BLOCK);
            }
        } else {
            for (;;) {
                rbs = get_counters(&produced, &consumed, nullptr);
                if (rbs != ringbuffer_status::OK)
                    return static_cast<long>(rbs);

                free_elements = CAPACITY - (produced - consumed);
                if (free_elements > 0)
                    break; /* leave the loop if we have room for new data */

                m_writing_semaphore.wait(); /* let's wait until consumer will read some data */
            }
        }

        if (count > free_elements)
            count = free_elements;

        if constexpr (is_power_of_two(CAPACITY))
            write_idx = produced & (CAPACITY - 1);
        else
            write_idx = produced % CAPACITY;

        split = ((write_idx + count) > CAPACITY) ? (CAPACITY - write_idx) : 0;
        remaining = count;

        if (split > 0) {
            if (false == xfer(m_buffer + write_idx, data, split))
                return static_cast<long>(ringbuffer_status::INTERNAL_ERROR);

            data += split;
            remaining -= split;
            write_idx = 0;
        }

        if (false == xfer(m_buffer + write_idx, data, remaining))
            return static_cast<long>(ringbuffer_status::INTERNAL_ERROR);

        m_counters.m_produced.store(produced + count, std::memory_order_relaxed);

        if (!NON_BLOCKING)
            m_reading_semaphore.post(); /* wake up one thread waiting for new data (if any) */

        return count;
    }

    template<typename DST, typename SRC>
    long read(DST data, std::size_t count, bool (*xfer)(DST, SRC, std::size_t))
    {
        std::size_t produced;
        std::size_t consumed;
        std::size_t available_elements;
        std::size_t read_idx;
        std::size_t split;
        std::size_t remaining;
        ringbuffer_status rbs;

        if (0 == count)
            return 0;

        if (NON_BLOCKING) {
            rbs = get_counters(&produced, &consumed, nullptr);
            if (rbs != ringbuffer_status::OK)
                return static_cast<long>(rbs);

            available_elements = produced - consumed;
            if (0 == available_elements) {
               return static_cast<long>(ringbuffer_status::WOULD_BLOCK);
            }
        } else {
            for (;;) {
                rbs = get_counters(&produced, &consumed, nullptr);
                if (rbs != ringbuffer_status::OK)
                    return static_cast<long>(rbs);

                available_elements = produced - consumed;
                if (available_elements > 0)
                    break; /* leave the loop if we have elements to be read */

                m_reading_semaphore.wait(); /* let's wait until producer will write some data */
            }
        }

        if (count > available_elements)
            count = available_elements;

        read_idx = consumed % CAPACITY;

        split = ((read_idx + count) > CAPACITY) ? (CAPACITY - read_idx) : 0;
        remaining = count;

        if (split > 0) {
            if (false == xfer(data, m_buffer + read_idx, split))
                return static_cast<long>(ringbuffer_status::INTERNAL_ERROR);

            data += split;
            remaining -= split;
            read_idx = 0;
        }

        if (false == xfer(data, m_buffer + read_idx, remaining))
            return static_cast<long>(ringbuffer_status::INTERNAL_ERROR);

        m_counters.m_consumed.store(consumed + count, std::memory_order_relaxed);

        if (!NON_BLOCKING)
            m_writing_semaphore.post(); /* wake up one thread waiting for some space in the buffer (if any) */

        return count;
    }

    struct alignas(CACHELINE_SIZE) counters
    {
        explicit counters()
        {
            reset();
        }

        void reset()
        {
            m_produced.store(0U, std::memory_order_relaxed);
            m_consumed.store(0U, std::memory_order_relaxed);
            m_dropped.store(0U, std::memory_order_relaxed);
        }

        std::string to_string() const
        {
            std::ostringstream stream;

            stream << "[";
            stream << "produced: ";
            stream << std::dec << m_produced.load(std::memory_order_relaxed);
            stream << ", ";
            stream << "consumed: ";
            stream << std::dec << m_consumed.load(std::memory_order_relaxed);
            stream << ", ";
            stream << "dropped: ";
            stream << std::dec << m_dropped.load(std::memory_order_relaxed);
            stream << "]";

            return stream.str();
        }

        operator std::string () const
        {
            return to_string();
        }

        std::atomic<std::size_t> m_produced;
        std::atomic<std::size_t> m_consumed;
        std::atomic<std::size_t> m_dropped;
    };

    counters m_counters;
    T* m_buffer;
    binary_semaphore m_writing_semaphore;
    binary_semaphore m_reading_semaphore;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

#endif /* _RINGBUFFER_HPP_ */
