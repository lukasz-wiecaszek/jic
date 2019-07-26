/**
 * @file parser_mpeg2ts.hpp
 *
 * Definition of MPEG2 Transport Stream (ISO/IEC 13818-1) parser.
 *
 * MPEG2 Transport Stream syntax and semantics is defined in ISO/IEC 13818-1
 * "Information technology — Generic coding of moving pictures and associated
 * audio information: Systems".
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

#ifndef _PARSER_MPEG2TS_HPP_
#define _PARSER_MPEG2TS_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>
#include <functional>

#include <cstring>
#include <climits>

#if defined(DEBUG_PARSERS)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "mpeg2ts_pid.hpp"
#include "../base/parser_base.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PARSER_MPEG2TS_PACKET_SIZE       188

#define PARSER_MPEG2TS_STATUSES \
    PARSER_MPEG2TS_STATUS(NEED_BYTES) \
    PARSER_MPEG2TS_STATUS(SYNC_GAINED) \
    PARSER_MPEG2TS_STATUS(SYNCHRONIZED) \
    PARSER_MPEG2TS_STATUS(SYNC_LOST) \
    PARSER_MPEG2TS_STATUS(NOT_SYNCHRONIZED) \
    PARSER_MPEG2TS_STATUS(TRANSPORT_RATE_DETECTED) \

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

enum class parser_mpeg2ts_status_e : int32_t
{
#define PARSER_MPEG2TS_STATUS(id) id,
    PARSER_MPEG2TS_STATUSES
#undef PARSER_MPEG2TS_STATUS
};

class parser_mpeg2ts : public parser_base<uint8_t>
{
public:
    explicit parser_mpeg2ts(std::size_t capacity) :
        parser_base<uint8_t>{capacity},
        m_state_function{&parser_mpeg2ts::waiting_for_sync},
        m_tspacket_size{0},
        m_transport_rate{}
    {
#if defined(DEBUG_PARSERS)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    virtual ~parser_mpeg2ts()
    {
#if defined(DEBUG_PARSERS)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    parser_mpeg2ts(const parser_mpeg2ts&) = delete;
    parser_mpeg2ts(parser_mpeg2ts&&) = delete;
    parser_mpeg2ts& operator = (const parser_mpeg2ts&) = delete;
    parser_mpeg2ts& operator = (parser_mpeg2ts&&) = delete;

    void reset()
    {
        parser_base::reset();
        m_state_function = &parser_mpeg2ts::waiting_for_sync;
        m_tspacket_size = 0;
        m_transport_rate.reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << parser_base::to_string();
        stream << std::endl;
        stream << "parser_mpeg2ts@";
        stream << std::hex << this;
        stream << " [m_tspacket_size: ";
        stream << std::dec << m_tspacket_size;
        stream << ", ";
        stream << "m_tspacket_rate: ";
        stream << std::dec << m_transport_rate;
        stream << "]";

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    parser_mpeg2ts_status_e parse()
    {
        return (this->*m_state_function)();
    }

    const uint8_t* get_tspacket()
    {
        const uint8_t *p;

        p = m_flatbuffer.read_ptr();
        m_flatbuffer.consume(m_tspacket_size);

        return p;
    }

    std::size_t get_tspacket_size() const
    {
        return m_tspacket_size;
    }

    std::size_t get_tspacket_rate() const
    {
        return m_transport_rate;
    }

private:
    int synchronize(const std::size_t tspacket_size);

    parser_mpeg2ts_status_e waiting_for_sync();
    parser_mpeg2ts_status_e synchronized();

    parser_mpeg2ts_status_e (parser_mpeg2ts::*m_state_function)();
    std::size_t m_tspacket_size;

    struct transport_rate
    {
        explicit transport_rate() :
            m_pcr_pid(MPEG2TS_PID_INVALID),
            m_pcr1(0),
            m_pcr2(0),
            m_no_of_packets(0),
            m_transport_rate(0)
        {
        }

        bool is_valid() const
        {
            return m_transport_rate != 0;
        }

        void reset()
        {
            m_pcr_pid = MPEG2TS_PID_INVALID;
            m_pcr1 = 0;
            m_pcr2 = 0;
            m_no_of_packets = 0;
            m_transport_rate = 0;
        }

        operator std::size_t () const
        {
            return m_transport_rate;
        }

        bool calculate(const uint8_t* tsp);

    private:
        mpeg2ts_pid m_pcr_pid;
        uint64_t m_pcr1;
        uint64_t m_pcr2;
        uint64_t m_no_of_packets;
        std::size_t m_transport_rate; /* ts packets per second */
    };

    transport_rate m_transport_rate;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{

inline mpeg2ts_pid parser_mpeg2ts_get_pid(const uint8_t* tsp)
{
   return mpeg2ts_pid(((tsp[1] << 8) + tsp[2]) & 0x1fff);
}

inline bool parser_mpeg2ts_has_pcr(const uint8_t* tsp)
{
   return ((tsp[3] & 0x20)  &&  // if adaptation field is present
           (tsp[4])         &&  // and has non-zero value
           (tsp[5] & 0x10));    // and if PCR flag is present
}

inline uint64_t parser_mpeg2ts_get_pcr(const uint8_t* tsp)
{
   uint64_t pcr_base = (((uint64_t)((tsp[6] << 24) + (tsp[7] << 16) + (tsp[8] << 8) + tsp[9]) << 8) | tsp[10]) >> 7;
   return 1ULL + pcr_base * 300 + (((tsp[10] << 8) | tsp[11]) & 0x1ff);
}

constexpr const char* to_string(parser_mpeg2ts_status_e e)
{
    const char *str = "invalid 'parser_mpeg2ts_status_e' value";

    switch (e) {
#define PARSER_MPEG2TS_STATUS(id) case parser_mpeg2ts_status_e::id: str = #id; break;
            PARSER_MPEG2TS_STATUSES
#undef PARSER_MPEG2TS_STATUS
    }

    return str;
}

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

#endif /* _PARSER_MPEG2TS_HPP_ */
