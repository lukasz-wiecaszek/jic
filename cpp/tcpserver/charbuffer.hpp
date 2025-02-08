/* SPDX-License-Identifier: MIT */
/**
 * @file charbuffer.hpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _CHARBUFFER_HPP_
#define _CHARBUFFER_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "flatbuffer.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global types definitions
\*===========================================================================*/
namespace lts
{

/**
 * @brief charbuffer
 *
 * This class represents continuous sequence of characters stored in flatbuffer.
 * Internal buffer used to store characters can be resized automatically,
 * if the current size is not enough to store new incoming characters.
 */
class charbuffer : public flatbuffer<char, true> {
public:
    /**
     * @brief Creates a charbuffer object.
     */
    explicit charbuffer(std::size_t capacity)
    : flatbuffer(capacity)
    {
    }

    ~charbuffer() override = default;

    // charbuffer object shall be non-copyable and non-movable
    charbuffer(const charbuffer&) = delete;
    charbuffer(charbuffer&&) = delete;
    charbuffer& operator=(const charbuffer&) = delete;
    charbuffer& operator=(charbuffer&&) = delete;

    /**
     * @brief Returns pointer to the '\0' terminated line found in this charbuffer.
     *        If no line is found, nullptr will be returned.
     *
     * @retrun Returns pointer to the '\0' terminated line or nullptr
     *         if no line is found in this charbuffer.
     */
    const char* getline(std::size_t* len = nullptr)
    {
        const char* line = nullptr;

        for (std::size_t i = m_counters.m_read_idx; i < m_counters.m_write_idx; ++i) {
            if ('\n' == m_buffer[i]) {
                std::size_t count = i - m_counters.m_read_idx + 1;
                m_buffer[i] = '\0';

                // remove DOS line ending (carriage return character)
                if ((count > 1) && ('\r' == m_buffer[i - 1])) {
                    m_buffer[i - 1] = '\0';
                }

                line = read_ptr();
                consume(count);
                if (len)
                    *len = count;

                break;
            }
        }

        return line;
    }
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

#endif /* _CHARBUFFER_HPP_ */
