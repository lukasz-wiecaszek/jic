/* SPDX-License-Identifier: MIT */
/**
 * @file flatbuffer.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _FLATBUFFER_HPP_
#define _FLATBUFFER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

/**
 * @brief flatbuffer
 *
 * This class implements flatbuffer design pattern.
 *
 * This implementation is deliberately not thread safe,
 * so if you want to use it from different threads,
 * you must ensure proper handling.
 */
template<typename T, bool RESIZE_ON_WRITE = false>
class flatbuffer {
public:
    /**
     * @brief Creates a flatbuffer object.
     *
     * @param[in] capacity Initial capacity (expressed in number of objects)
     *                     of the flatbuffer.
     */
    explicit flatbuffer(std::size_t capacity)
    : m_capacity{capacity}
    , m_counters{capacity}
    , m_buffer{nullptr}
    {
        m_buffer = static_cast<T*>(::malloc(m_capacity * sizeof(T)));
    }

    virtual ~flatbuffer()
    {
        ::free(m_buffer);
    }

    // flatbuffer object shall be non-copyable and non-movable
    flatbuffer(const flatbuffer&) = delete;
    flatbuffer(flatbuffer&&) = delete;
    flatbuffer& operator=(const flatbuffer&) = delete;
    flatbuffer& operator=(flatbuffer&&) = delete;

    /**
     * @brief Returns true if the flatbuffer object is valid (was properly constructed),
     *        false otherwise.
     *
     * @retrun true if the flatbuffer object is valid (was properly constructed), false otherwise.
     */
    bool is_valid() const
    {
        return (m_capacity > 0) && (nullptr != m_buffer);
    }

    /**
     * @brief Returns actual capacity of the flatbuffer.
     *
     * @retrun Actual capacity of the flatbuffer.
     */
    std::size_t capacity() const
    {
        return m_capacity;
    }

    /**
     * @brief Returns maximum number of T objects which can be successfully
     *        written to the flatbuffer.
     *
     * @retrun Maximum number of T objects which can be successfully
     *         written to the flatbuffer.
     */
    std::size_t write_available() const
    {
        return m_counters.m_write_avail;
    }

    /**
     * @brief Returns number of T objects available to be read from the flatbuffer.
     *
     * @retrun Number of T objects available to be read from the flatbuffer.
     */
    std::size_t read_available() const
    {
        return m_counters.m_read_avail;
    }

    /**
     * @brief Returns pointer to the write end in the flatbuffer.
     *
     * @retrun Pointer to the write end in the flatbuffer.
     */
    T* write_ptr() const
    {
        return m_buffer + m_counters.m_write_idx;
    }

    /**
     * @brief Returns const pointer to the read end in the flatbuffer.
     *
     * @retrun Const pointer to the read end in the flatbuffer.
     */
    const T* read_ptr() const
    {
        return m_buffer + m_counters.m_read_idx;
    }

    /**
     * @brief Moves write pointer to the element which is placed 'count' T objects
     *        ahead of the current write pointer efectively producing 'count' T objects
     *        in the flatbuffer.
     *
     * @retrun Number of T objects actually produced.
     */
    std::size_t produce(std::size_t count)
    {
        if (count > m_counters.m_write_avail) {
            if (false == RESIZE_ON_WRITE) {
                count = m_counters.m_write_avail;
            } else {
                resize(((count / m_capacity) + 1) * m_capacity);
            }
        }

        m_counters.m_write_idx += count;
        m_counters.m_write_avail -= count;
        m_counters.m_read_avail += count;

        return count;
    }

    /**
     * @brief Moves read pointer to the element which is placed 'count' T objects
     *        ahead of the current read pointer efectively consuming 'count' T objects
     *        from the flatbuffer.
     *
     * @retrun Number of T objects actually consumed.
     */
    std::size_t consume(std::size_t count)
    {
        if (count > m_counters.m_read_avail) {
            count = m_counters.m_read_avail;
        }

        m_counters.m_read_idx += count;
        m_counters.m_read_avail -= count;

        return count;
    }

    /**
     * @brief Sets a bookmark in the flatbuffer.
     *
     * @retrun none
     */
    void set_bookmark()
    {
        m_counters.m_bookmark_idx = m_counters.m_read_idx;
    }

    /**
     * @brief Returns const pointer to the object in the flatbuffer
     *        pointed to by previously set bookmark.
     *
     * @retrun Const pointer to the object in the flatbuffer
     *         pointed to by previously set bookmark or nullptr if no bookmark is set.
     */
    const T* get_bookmark() const
    {
        return (m_counters.m_bookmark_idx >= 0) ? m_buffer + m_counters.m_bookmark_idx : nullptr;
    }

    /**
     * @brief Clears (unsets) the bookmark.
     *
     * @retrun none
     */
    void clear_bookmark()
    {
        m_counters.m_bookmark_idx = -1;
    }

    /**
     * @brief Resets the flatbuffer.
     *
     * @retrun none
     */
    void reset()
    {
        m_counters.reset(m_capacity);
    }

    /**
     * @brief Copies unread data to the begining of the flatbuffer.
     *        If bookmark is set, then unread data starts at location
     *        pointed to by bookmark. Otherwise, unread data start at next
     *        read location.
     *
     * @retrun none
     */
    void move()
    {
        std::size_t x;

        if (0 <= m_counters.m_bookmark_idx) {
            x = m_counters.m_bookmark_idx;
            m_counters.m_bookmark_idx = 0;
        } else {
            x = m_counters.m_read_idx;
        }

        if (0 < x) {
            std::size_t available_elements = m_counters.m_write_idx - x;
            std::size_t offset = m_counters.m_read_idx - x;

            std::memmove(m_buffer, &m_buffer[x], available_elements * sizeof(T));

            m_counters.m_read_idx = offset;
            m_counters.m_write_idx = available_elements;
            m_counters.m_write_avail = m_capacity - available_elements;
        }
    }

    /**
     * @brief Attempts to write 'count' T objects to the flatbuffer
     *        from the array pointed to by 'data'.
     *
     * @param[in] data Array of T objects.
     * @param[in] count Number of T objects to be written.
     *
     * @return Number of T objects actually written to the flatbuffer.
     */
    std::size_t write(const T data[], std::size_t count)
    {
        if (count > m_counters.m_write_avail) {
            if (false == RESIZE_ON_WRITE) {
                count = m_counters.m_write_avail;
            } else {
                resize(((count / m_capacity) + 1) * m_capacity);
            }
        }

        if ((0 < count) && (nullptr != m_buffer)) {
            std::memcpy(&m_buffer[m_counters.m_write_idx], data, count * sizeof(T));

            m_counters.m_write_idx += count;
            m_counters.m_write_avail -= count;
            m_counters.m_read_avail += count;
        }

        return count;
    }

    /**
     * @brief Attempts to write 1 T objects to the flatbuffer.
     *
     * @param[in] data Reference to the T object to be written to the flatbuffer.
     *
     * @return Number of T objects actually written to the flatbuffer.
     */
    std::size_t write(const T& data)
    {
        return write(&data, 1);
    }

    /**
     * @brief Attempts to write 'N' T objects to the flatbuffer from the array
     *        pointed to by 'data'.
     *
     * @param[in] data Array of T objects.
     *
     * @return Number of T objects actually written to the flatbuffer.
     */
    template<std::size_t N>
    std::size_t write(const T (&data)[N])
    {
        return write(data, N);
    }

    /**
     * @brief Returns a textual representation of an object.
     *
     * @return A textual representation of an object.
     */
    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "flatbuffer@";
        stream << std::hex << this;
        stream << " [capacity: ";
        stream << std::dec << m_capacity;
        stream << ", ";
        stream << static_cast<std::string>(m_counters);
        stream << "]";

        return stream.str();
    }

    /**
     * @brief Returns a textual representation of an object.
     *
     * @return A textual representation of an object.
     */
    operator std::string() const
    {
        return to_string();
    }

protected:
    /**
     * @brief Increases the capacity of the flatbuffer (the total number of elements
     *        that the flatbuffer can hold without requiring reallocation)
     *        by the 'resize' elements.
     */
    void resize(std::size_t resize)
    {
        m_capacity += resize;
        m_counters.m_write_avail += resize;

        m_buffer = static_cast<T*>(realloc(m_buffer, m_capacity * sizeof(T)));
    }

    /**
     * @brief counters helper class
     */
    struct counters {
        /**
         * @brief Creates object.
         * @param capacity Initial capacity.
         */
        counters(std::size_t capacity)
        {
            reset(capacity);
        }

        /**
         * @brief Resets objects members.
         * @param capacity Initial capacity.
         */
        void reset(std::size_t capacity)
        {
            m_read_idx = 0;
            m_read_avail = 0;
            m_write_idx = 0;
            m_write_avail = capacity;
            m_bookmark_idx = -1;
        }

        /**
         * @brief Creates string representation of object.
         */
        std::string to_string() const
        {
            std::ostringstream stream;

            stream << "[";
            stream << "read_idx: ";
            stream << m_read_idx;
            stream << ", ";
            stream << "write_idx: ";
            stream << m_write_idx;
            stream << ", ";
            stream << "bookmark_idx: ";
            stream << m_bookmark_idx;
            stream << ", ";
            stream << "read_avail: ";
            stream << m_read_avail;
            stream << ", ";
            stream << "write_avail: ";
            stream << m_write_avail;
            stream << "]";

            return stream.str();
        }

        /**
         * @brief Creates string representation of object.
         */
        operator std::string() const
        {
            return to_string();
        }

        /**
         * @brief Read index.
         */
        std::size_t m_read_idx;

        /**
         * @brief Size available to read.
         */
        std::size_t m_read_avail;

        /**
         * @brief Write index.
         */
        std::size_t m_write_idx;

        /**
         * @brief Size available to write.
         */
        std::size_t m_write_avail;

        /**
         * @brief Bookmark index.
         */
        long m_bookmark_idx; /* -1 when not set */
    };

    /**
     * @brief Capacity of buffer.
     */
    std::size_t m_capacity;

    /**
     * @brief Counters object.
     */
    counters m_counters;

    /**
     * @brief Underlying buffer.
     */
    T* m_buffer;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{
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

#endif /* _FLATBUFFER_HPP_ */
