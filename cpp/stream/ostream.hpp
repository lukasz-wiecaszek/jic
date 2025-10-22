/* SPDX-License-Identifier: MIT */
/**
 * @file ostream.hpp
 *
 * Definition of output stream class.
 *
 * It contains methods used to serialize higher level data types
 * into a sequence of bytes. The serialization process can be done
 * according to the little or big endian byte ordering rules.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _OSTREAM_HPP_
#define _OSTREAM_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstdint>
#include <cassert>
#include <cstring>

#include <string>
#include <sstream>
#include <functional> /* for std::function */

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <utilities.hpp>
#include <stream_position.hpp>
#include <endianness.hpp>
#include <structure.hpp>
#include <ilog2.hpp>

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define OSTREAM_BYTE_ORDER_BIG_ENDIAN        (0)
#define OSTREAM_BYTE_ORDER_LITTLE_ENDIAN     (1)

/** no error was encountered during stream handling */
#define OSTREAM_STATUS_OK                    (0)
/** end of stream was reached */
#define OSTREAM_STATUS_EOS_REACHED           (1 << 0)
/** an attempt to write to not byte aligned position was spotted */
#define OSTREAM_STATUS_IMPROPER_ALIGMENT     (1 << 1)
/** stream was marked as corrupted by user */
#define OSTREAM_STATUS_STREAM_CORRUPTED      (1 << 2)

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

enum class ostream_seek_e : int32_t
{
    /** current position */
    CURRENT,
    /** beginning of a stream */
    BEGINNING,
    /** end of a stream */
    END,
};

typedef std::function<void(const stream_position& before, const stream_position& after)> ostream_error_function;

template<int STREAM_BYTE_ORDER>
class ostream
{
public:
    ostream() :
        m_buffer{nullptr},
        m_size{0},
        m_position{},
        m_status{OSTREAM_STATUS_OK},
        m_error_function{nullptr}
    {
    }

    ostream(uint8_t* buffer, std::size_t size, const ostream_error_function& function = nullptr) :
        m_buffer{buffer},
        m_size{size},
        m_position{},
        m_status{OSTREAM_STATUS_OK},
        m_error_function{function}
    {
    }

    ~ostream()
    {
    }

    ostream(const ostream& other) :
        m_buffer(other.m_buffer),
        m_size(other.m_size),
        m_position(other.m_position),
        m_status(other.m_status),
        m_error_function(other.m_error_function)
    {
    }

    ostream& operator = (const ostream& other)
    {
        m_buffer         = other.m_buffer;
        m_size           = other.m_size;
        m_position       = other.m_position;
        m_status         = other.m_status;
        m_error_function = other.m_error_function;

        return *this;
    }

    bool is_valid() const
    {
        return m_buffer != nullptr;
    }

    uint32_t status() const
    {
        return m_status;
    }

    void mark_corrupted()
    {
       m_status |= OSTREAM_STATUS_STREAM_CORRUPTED;
    }

    void clear_status()
    {
        m_status = OSTREAM_STATUS_OK;
    }

    void set_error_function(const ostream_error_function& function)
    {
        m_error_function = function;
    }

    void clear_error_function()
    {
        m_error_function = nullptr;
    }

    std::size_t size() const
    {
        return m_size;
    }

    uint8_t* data() const
    {
        return m_buffer;
    }

    const uint8_t* current_data_pointer() const
    {
        return m_buffer + m_position.m_byte_position;
    }

    bool write_u8(const uint8_t& value)
    {
        return write(value);
    }

    bool write_s8(const int8_t& value)
    {
        return write(value);
    }

    bool write_u16(const uint16_t& value)
    {
        uint16_t v = to_stream_byte_order(value);
        return write(v);
    }

    bool write_s16(const int16_t& value)
    {
        int16_t v = to_stream_byte_order(value);
        return write(v);
    }

    bool write_u32(const uint32_t& value)
    {
        uint32_t v = to_stream_byte_order(value);
        return write(v);
    }

    bool write_s32(const int32_t& value)
    {
        int32_t v = to_stream_byte_order(value);
        return write(v);
    }

    bool write_u64(const uint64_t& value)
    {
        uint64_t v = to_stream_byte_order(value);
        return write(v);
    }

    bool write_s64(const int64_t& value)
    {
        int64_t v = to_stream_byte_order(value);
        return write(v);
    }

    /**
     * Writes 'number_of_bits' bits from T value into the current stream position.
     *
     * If the write operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[in] number_of_bits Number of bits to be written.
     * @param[in] value 'T' representation of the bit pattern to be written into the stream.
     *
     * @return true on success, false on failure.
     */
    template<typename T>
    bool write_bits(uint32_t number_of_bits, const T& value)
    {
        static_assert(std::is_integral<T>::value, "T must be an integral type");

        if (unlikely(number_of_bits > sizeof(T) * 8))
            return false;

        /* verify whether we don't go beyond size of the stream */
        if ((m_position.to_bits() + number_of_bits) <= (m_size * BITS_PER_BYTE)) {

            uint8_t v;
            uint32_t bits_remaining = number_of_bits;
            uint32_t bits_to_write;

            while (bits_remaining > 0) {
                /* there are more bits to be written than bits left in the stream's current byte */
                if ((m_position.m_bit_position + bits_remaining) >= BITS_PER_BYTE) {
                    /* number of bits left in the stream's current byte */
                    bits_to_write = BITS_PER_BYTE - m_position.m_bit_position;

                    /* prepare bit pattern to be written into the stream */
                    v = (value >> (bits_remaining - bits_to_write)) & ((1U << bits_to_write) - 1);

                    /* and finally write it */
                    m_buffer[m_position.m_byte_position] |= v;

                    /* move to the next byte in the stream */
                    m_position.m_byte_position++;
                    /* and reset bit position to the begining of a byte */
                    m_position.m_bit_position = 0;
                }
                else {
                    /* there remains less than 8 bits to be written */
                    bits_to_write = bits_remaining;

                    /* advance bit position */
                    m_position.m_bit_position += bits_to_write;

                    /* prepare bit pattern to be written into the stream */
                    v = (value & ((1U << bits_to_write) - 1)) << (BITS_PER_BYTE - m_position.m_bit_position);

                    /* and finally write it */
                    m_buffer[m_position.m_byte_position] |= v;
                }

                /* decrease number of remaining bits by the number of bits written in this iteration */
                bits_remaining -= bits_to_write;
            }

            return true;
       }
       else {
            m_status |= OSTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + number_of_bits);
            return false;
       }
    }

    /**
     * Writes 'value' as an unsigned Exp-Golomb bit string.
     *
     * If the write operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[in] value 'uint32_t' value to be represented in the stream
     *                   as an unsigned Exp-Golomb bit string.
     *
     * @note The caller must ensure that the value is less then (2^32 - 1).
     *
     * @return true on success, false on failure.
     */
    bool write_exp_golomb_u(const uint32_t& value)
    {
        int e = ilog2(value + 1);
        return write_bits(e * 2 + 1, value + 1);
    }

    /**
     * Writes 'value' as an signed Exp-Golomb bit string.
     *
     * If the write operation would cause that stream position is moved beyond
     * the stream boundaries, then this function fails (returns false)
     * and stream position is not altered.
     *
     * @param[in] value 'int16_t' value to be represented in the stream
     *                   as an signed Exp-Golomb bit string.
     *
     *
     * @return true on success, false on failure.
     */
    bool write_exp_golomb_s(const int16_t& value)
    {
        int32_t v = value * 2 - 1;
        if (v < 0)
            v ^= -1;
        return write_exp_golomb_u(v);
    }

    /**
     * Writes structure/tuple of data.
     *
     * @param[in] value 'structure<TYPES...>' to be written (serialized) to the stream.
     *
     * @return true on success, false on failure.
     */
    template<typename... TYPES>
    bool write_structure(const structure<TYPES...>& value)
    {
        const std::size_t size = sizeof...(TYPES);
        if (size == 0)
            return true;

        return structure_writer<size - 1, ostream<STREAM_BYTE_ORDER>, structure<TYPES...>>{}(*this, value);
    }

    /**
     * Skips current position in the stream by a specified number of bytes.
     *
     * You can use 'skip()' method to skip any number of bytes, even beyond
     * the end of the stream. No error will be reported in such a situation,
     * but when the stream position is beyond the stream boundaries,
     * then subsequent read and peek operations will fail.
     *
     * @param[in] number_of_bytes Number of bytes to skip.
     *
     * @return none
     */
    void skip(std::size_t number_of_bytes)
    {
        m_position.m_byte_position += number_of_bytes;
    }

    /**
     * Skips current position in the stream by a specified number of bits.
     *
     * You can use 'skip_bits()' method to skip given number of bits, even beyond
     * the end of the stream. No error will be reported in such a situation,
     * but when the stream position is beyond the stream boundaries,
     * then subsequent read and peek operations will fail.
     *
     * @param[in] number_of_bits Number of bits to skip.
     *
     * @return none
     */
    void skip_bits(std::size_t number_of_bits)
    {
        m_position += number_of_bits;
    }

    /**
     * Gets the current position of a stream.
     *
     * The position is expressed as an offset (given in bytes)
     * relative to the beginning of the stream.
     *
     * @return Current position of a stream.
     */
    std::size_t tell() const
    {
        return m_position.m_byte_position;
    }

    /**
     * Gets the current bit position of a stream.
     *
     * The bit position is expressed as an offset (given in bits)
     * within a byte where subsequent write operation will start from.
     * Thus the bit position reaches the values from
     * 0 (most significant bit) till 7 (least significant bit).
     *
     * @return Current bit position of a stream.
     */
    std::size_t tell_bits() const
    {
        return m_position.m_bit_position;
    }

    /**
     * Moves the stream position to a specified location.
     *
     * The location is calculated by adding an offset (given in bytes)
     * to the 'origin'. The next operation on the stream will take place
     * at the new location.
     * You can use 'seek()' method to reposition the position anywhere in a stream.
     * The position can also be positioned beyond the end of the stream.
     * No error will be reported by this method, but when the stream position
     * is beyond the stream boundaries, then subsequent read and peak operations will fail.
     *
     * @param[in] origin Initial position.
     * @param[in] offset Specifies number of bytes from origin.
     */
    void seek(ostream_seek_e origin, long offset)
    {
        switch (origin) {
            case ostream_seek_e::CURRENT:
                m_position.m_byte_position += offset;
                break;

            case ostream_seek_e::BEGINNING:
                m_position.m_byte_position = offset;
                break;

            case ostream_seek_e::END:
                m_position.m_byte_position = m_size + offset;
                break;

            default:
                break;
        }
    }

    /**
     * Gets number of bytes which remain unread in the stream.
     *
     * @return Number of bytes which remain unread in the stream.
     */
    long remains() const
    {
        return m_size - m_position.m_byte_position;
    }

    /**
     * Sets the position in the stream at the beginning.
     *
     * @return none
     */
    void rewind()
    {
        m_position.reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << "ostream@";
        stream << std::hex << this;
        stream << " [";
        stream << "size: ";
        stream << std::dec << m_size;
        stream << ", position: ";
        stream << static_cast<std::string>(m_position);
        stream << ", status: ";
        stream << status_to_string(m_status);
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    static std::string status_to_string(uint32_t status)
    {
        if (status == OSTREAM_STATUS_OK)
            return std::string{"ok"};
        else {
            std::ostringstream stream;

            if (status & OSTREAM_STATUS_EOS_REACHED) {
                stream << "eos";
                if (status >= (OSTREAM_STATUS_EOS_REACHED << 1))
                    stream << ", ";
            }

            if (status & OSTREAM_STATUS_IMPROPER_ALIGMENT) {
                stream << "aligment";
                if (status >= (OSTREAM_STATUS_IMPROPER_ALIGMENT << 1))
                    stream << ", ";
            }

            if (status & OSTREAM_STATUS_STREAM_CORRUPTED) {
                stream << "corrupted";
                if (status >= (OSTREAM_STATUS_STREAM_CORRUPTED << 1))
                    stream << ", ";
            }

            return stream.str();
        }
    }

private:
   template<typename T>
   constexpr T to_stream_byte_order(T x)
   {
       if constexpr (STREAM_BYTE_ORDER == OSTREAM_BYTE_ORDER_BIG_ENDIAN)
           return cputobe(x);
       else
       if constexpr (STREAM_BYTE_ORDER == OSTREAM_BYTE_ORDER_LITTLE_ENDIAN)
          return cputole(x);
       else
           static_assert(
               (STREAM_BYTE_ORDER == OSTREAM_BYTE_ORDER_BIG_ENDIAN) ||
               (STREAM_BYTE_ORDER == OSTREAM_BYTE_ORDER_LITTLE_ENDIAN));
   }

    template<typename T>
    bool write(const T& value)
    {
        if (unlikely(m_position.m_bit_position)) {
            m_status |= OSTREAM_STATUS_IMPROPER_ALIGMENT;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
            return false;
        }

        bool status = writebytes(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
        if (unlikely(!status)) {
            m_status |= OSTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + sizeof(T) * BITS_PER_BYTE);
        }

        return status;
    }

    bool align(const std::size_t size)
    {
        const std::size_t mask = size - 1;
        const std::size_t bytepos = (m_position.m_byte_position + mask) & (~mask);

        if (unlikely(m_position.m_bit_position)) {
            m_status |= OSTREAM_STATUS_IMPROPER_ALIGMENT;
            if (m_error_function)
                m_error_function(m_position, m_position + bytepos * BITS_PER_BYTE);
            return false;
        }

        if (likely(bytepos <= m_size)) {
            m_position.m_byte_position = bytepos;
            return true;
        }
        else {
            m_status |= OSTREAM_STATUS_EOS_REACHED;
            if (m_error_function)
                m_error_function(m_position, m_position + bytepos * BITS_PER_BYTE);
            return false;
        }
    }

    bool writebytes(const uint8_t* data, const std::size_t size)
    {
        bool retval = false;

        do {
            const std::size_t pos = m_position.m_byte_position + size;
            if (pos > m_size)
                break;

            std::memcpy(m_buffer + m_position.m_byte_position, data, size);
            m_position.m_byte_position = pos;
            retval = true;
        } while (0);

        return retval;
    }

    /** A pointer to the memory buffer constituing a stream data. */
    uint8_t* m_buffer;

    /** Only bytes whitin the range m_buffer[0] ... m_buffer[m_size - 1]
    are valid stream data. */
    std::size_t m_size;

    /** Current stream position. */
    stream_position m_position;

    /** Stream's status */
    uint32_t m_status;

    /** Stores error function. Can be nullptr in which case
    error function is considered as not registered. */
    ostream_error_function m_error_function;
};

using ostream_be = ostream<OSTREAM_BYTE_ORDER_BIG_ENDIAN>;
using ostream_le = ostream<OSTREAM_BYTE_ORDER_LITTLE_ENDIAN>;

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

template<typename STREAM>
bool operator << (STREAM& stream, const uint8_t& value)
{
    return stream.template write_u8(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const int8_t& value)
{
    return stream.template write_s8(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const uint16_t& value)
{
    return stream.template write_u16(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const int16_t& value)
{
    return stream.template write_s16(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const uint32_t& value)
{
    return stream.template write_u32(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const int32_t& value)
{
    return stream.template write_s32(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const uint64_t& value)
{
    return stream.template write_u64(value);
}

template<typename STREAM>
bool operator << (STREAM& stream, const int64_t& value)
{
    return stream.template write_s64(value);
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

#endif /* _OSTREAM_HPP_ */
