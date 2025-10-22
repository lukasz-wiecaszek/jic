/* SPDX-License-Identifier: MIT */
/**
 * @file stream_position.hpp
 *
 * Helper class used to store current stream position.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _STREAM_POSITION_HPP_
#define _STREAM_POSITION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define BITS_PER_BYTE 8

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

struct stream_position
{
    constexpr explicit stream_position(std::size_t byte_position = 0, std::size_t bit_position = 0) :
        m_byte_position{byte_position},
        m_bit_position{bit_position}
    {
    }

    constexpr stream_position(const stream_position& other) :
        m_byte_position{other.m_byte_position},
        m_bit_position{other.m_bit_position}
    {
    }

    constexpr stream_position& operator = (const stream_position& other)
    {
        m_byte_position  = other.m_byte_position;
        m_bit_position   = other.m_bit_position;

        return *this;
    }

    constexpr bool operator == (const stream_position& other) const
    {
        return ((m_byte_position == other.m_byte_position) &&
                (m_bit_position  == other.m_bit_position));
    }

    constexpr bool operator != (const stream_position& other) const
    {
        return !(*this == other);
    }

    constexpr stream_position operator + (const stream_position& other) const
    {
        std::size_t number_of_bits = to_bits() + other.to_bits();

        stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

        return pos;
    }

    constexpr stream_position operator - (const stream_position& other) const
    {
        std::size_t number_of_bits = to_bits() - other.to_bits();

        stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

        return pos;
    }

    constexpr stream_position operator += (const stream_position& other)
    {
        std::size_t number_of_bits = to_bits() + other.to_bits();

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr stream_position operator -= (const stream_position& other)
    {
        std::size_t number_of_bits = to_bits() - other.to_bits();

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr stream_position operator += (std::size_t bits)
    {
        std::size_t number_of_bits = to_bits() + bits;

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr stream_position operator -= (std::size_t bits)
    {
        std::size_t number_of_bits = to_bits() - bits;

        m_byte_position = number_of_bits / BITS_PER_BYTE;
        m_bit_position = number_of_bits % BITS_PER_BYTE;

        return *this;
    }

    constexpr std::size_t to_bits() const
    {
        return m_byte_position * BITS_PER_BYTE + m_bit_position;
    }

    constexpr void reset()
    {
        m_byte_position = 0;
        m_bit_position = 0;
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << m_byte_position;
        stream << ":";
        stream << m_bit_position;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    /**
     * Stores position of the next byte to be read from or written to
     * the stream's buffer (relative to the beginning of the buffer).
     */
    std::size_t m_byte_position;

    /**
     * Stores position of the next bit to be read from or written to
     * the stream's buffer (relative to the current byte position).
     * Bit position is expressed as the offset from byte boundary,
     * most significiant bit first.
     * Thus bit position takes values from 0 (msb) till 7 (lsb).
     */
    std::size_t m_bit_position;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

constexpr stream_position operator + (const stream_position& other, std::size_t bits)
{
    std::size_t number_of_bits = other.to_bits() + bits;

    stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr stream_position operator + (std::size_t bits, const stream_position& other)
{
    std::size_t number_of_bits = bits + other.to_bits();

    stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr stream_position operator - (const stream_position& other, std::size_t bits)
{
    std::size_t number_of_bits = other.to_bits() + bits;

    stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

constexpr stream_position operator - (std::size_t bits, const stream_position& other)
{
    std::size_t number_of_bits = bits + other.to_bits();

    stream_position pos(number_of_bits / BITS_PER_BYTE, number_of_bits % BITS_PER_BYTE);

    return pos;
}

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

#endif /* _STREAM_POSITION_HPP_ */
