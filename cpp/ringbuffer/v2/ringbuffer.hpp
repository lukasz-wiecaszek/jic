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
#include <mutex>
#include <condition_variable>

#include <cassert>
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

template<typename T>
class ringbuffer
{
public:
    typedef T value_type;

    explicit ringbuffer(std::size_t capacity, bool non_blocking = false) :
        m_capacity{capacity},
        m_non_blocking{non_blocking},
        m_counters{},
        m_buffer{nullptr},
        m_writing_completion{},
        m_reading_completion{}
    {
        assert(capacity > 0);
        assert(capacity < LONG_MAX);

        m_buffer = new T[capacity];

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
        return m_capacity;
    }

    bool non_blocking() const
    {
        return m_non_blocking;
    }

    ringbuffer_status get_counters(std::size_t* produced, std::size_t* consumed, std::size_t* dropped) const
    {
        std::size_t l_produced = m_counters.m_produced.load(std::memory_order_relaxed);
        std::size_t l_consumed = m_counters.m_consumed.load(std::memory_order_relaxed);

        if (l_produced < l_consumed)
            return ringbuffer_status::INTERNAL_ERROR;

        if ((l_produced - l_consumed) > m_capacity) /* LONG_MAX is the max capacity */
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
        stream << std::dec << m_capacity;
        stream << ", ";
        stream << "policy: ";
        if  (m_non_blocking)
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

private:
    template<typename U>
    static void copy(U* dst, const U* src, std::size_t count)
    {
        while (count-- > 0)
            *dst++ = *src++;
    }

    template<typename U>
    static void move(U* dst, U* src, std::size_t count)
    {
        while (count-- > 0)
            *dst++ = std::move(*src++);
    }

    template<typename DST, typename SRC>
    long write(SRC data, std::size_t count, void (*xfer)(DST, SRC, std::size_t))
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

        if (m_non_blocking) {
            rbs = get_counters(&produced, &consumed, nullptr);
            if (rbs != ringbuffer_status::OK)
                return static_cast<long>(rbs);

            free_elements = m_capacity - (produced - consumed);
            if (0 == free_elements) {
                m_counters.m_dropped++;
                return static_cast<long>(ringbuffer_status::WOULD_BLOCK);
            }
        } else {
            std::unique_lock<decltype(m_writing_completion.m_mutex)> lock(m_writing_completion.m_mutex);

            for (;;) {
                rbs = get_counters(&produced, &consumed, nullptr);
                if (rbs != ringbuffer_status::OK)
                    return static_cast<long>(rbs);

                free_elements = m_capacity - (produced - consumed);
                if (free_elements > 0)
                    break; /* leave the loop if we have room for new data */
                m_writing_completion.wait(lock); /* let's wait until consumer will read some data */
            }
        }

        if (count > free_elements)
            count = free_elements;

        write_idx = produced % m_capacity;

        split = ((write_idx + count) > m_capacity) ? (m_capacity - write_idx) : 0;
        remaining = count;

        if (split > 0) {
            xfer(m_buffer + write_idx, data, split);

            data += split;
            remaining -= split;
            write_idx = 0;
        }

        xfer(m_buffer + write_idx, data, remaining);
        m_counters.m_produced.store(produced + count, std::memory_order_relaxed);

        if (!m_non_blocking)
            m_reading_completion.notify(); /* wake up one thread waiting for new data (if any) */

        return count;
    }

    template<typename DST, typename SRC>
    long read(DST data, std::size_t count, void (*xfer)(DST, SRC, std::size_t))
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

        if (m_non_blocking) {
            rbs = get_counters(&produced, &consumed, nullptr);
            if (rbs != ringbuffer_status::OK)
                return static_cast<long>(rbs);

            available_elements = produced - consumed;
            if (0 == available_elements) {
               return static_cast<long>(ringbuffer_status::WOULD_BLOCK);
            }
        } else {
            std::unique_lock<decltype(m_reading_completion.m_mutex)> lock(m_reading_completion.m_mutex);

            for (;;) {
                rbs = get_counters(&produced, &consumed, nullptr);
                if (rbs != ringbuffer_status::OK)
                    return static_cast<long>(rbs);

                available_elements = produced - consumed;
                if (available_elements > 0)
                    break; /* leave the loop if we have elements to be read */
                m_reading_completion.wait(lock); /* let's wait until producer will write some data */
            }
        }

        if (count > available_elements)
            count = available_elements;

        read_idx = consumed % m_capacity;

        split = ((read_idx + count) > m_capacity) ? (m_capacity - read_idx) : 0;
        remaining = count;

        if (split > 0) {
            xfer(data, m_buffer + read_idx, split);

            data += split;
            remaining -= split;
            read_idx = 0;
        }

        xfer(data, m_buffer + read_idx, remaining);
        m_counters.m_consumed.store(consumed + count, std::memory_order_relaxed);

        if (!m_non_blocking)
            m_writing_completion.notify(); /* wake up one thread waiting for some space in the buffer (if any) */

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

    struct completion
    {
        explicit completion() :
            m_mutex(),
            m_condvar(),
            m_condition(false)
        {
        }

        void wait(std::unique_lock<std::mutex>& lock)
        {
            m_condition = false;
            m_condvar.wait(lock);
        }

        void notify()
        {
            std::lock_guard<decltype(m_mutex)> lock(m_mutex);
            if (!m_condition) {
                m_condition = true;
                m_condvar.notify_one();
            }
        }

        std::mutex m_mutex;
        std::condition_variable m_condvar;
        bool m_condition;
    };

    std::size_t m_capacity;
    bool m_non_blocking;
    counters m_counters;
    T* m_buffer;
    completion m_writing_completion;
    completion m_reading_completion;
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
