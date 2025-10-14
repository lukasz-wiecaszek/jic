/**
 * @file parser_mpeg2ts.cpp
 *
 * Brief description of what's inside this source file.
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

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "mpeg2ts_pid.hpp"
#include "parser_mpeg2ts.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/
using namespace lts;

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PARSER_MPEG2TS_SYNC_BYTE                     0x47
#define PARSER_MPEG2TS_FORWARD_SCAN                    10
#define PARSER_MPEG2TS_NO_SYNC_INDICATOR               -1

#define PARSER_MPEG2TS_PACKET_SIZE_EXTENSIONS \
    X(REED_SOLOMON_CODE_LENGTH_DVB,  16) \
    X(REED_SOLOMON_CODE_LENGTH_ATSC, 20) \

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

enum mpeg2ts_parser_packet_size_extensions
{
#define X(a, b) a = b,
    PARSER_MPEG2TS_PACKET_SIZE_EXTENSIONS
#undef X
};

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/
static constexpr std::size_t mpeg2ts_parser_packet_sizes[] = {
    PARSER_MPEG2TS_PACKET_SIZE,
#define X(a, b) PARSER_MPEG2TS_PACKET_SIZE + a,
    PARSER_MPEG2TS_PACKET_SIZE_EXTENSIONS
#undef X
};

static constexpr std::size_t mpeg2ts_parser_packet_sizes_elements =
    sizeof(mpeg2ts_parser_packet_sizes) / sizeof(mpeg2ts_parser_packet_sizes[0]);

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
static constexpr std::size_t mpeg2ts_parser_bigest_packet_size()
{
    std::size_t max = 0;

    for (std::size_t n = 0; n < mpeg2ts_parser_packet_sizes_elements; ++n)
        if (mpeg2ts_parser_packet_sizes[n] > max)
            max = mpeg2ts_parser_packet_sizes[n];

    return max;
}

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
int parser_mpeg2ts::synchronize(const std::size_t tspacket_size)
{
    int offset;
    std::size_t i, j;
    const uint8_t *p;

    offset = PARSER_MPEG2TS_NO_SYNC_INDICATOR;
    p = m_flatbuffer.read_ptr();

    for (i = 0;
        ((i < tspacket_size) && (PARSER_MPEG2TS_NO_SYNC_INDICATOR == offset));
        ++i) {
        if (p[i] != PARSER_MPEG2TS_SYNC_BYTE) continue;
        offset = static_cast<int>(i);
        for (j = 1; j <= PARSER_MPEG2TS_FORWARD_SCAN; ++j) {
            if (p[j * tspacket_size + offset] != PARSER_MPEG2TS_SYNC_BYTE) {
                offset = PARSER_MPEG2TS_NO_SYNC_INDICATOR;
                break;
            }
        }
    }

    return offset;
}

parser_mpeg2ts_status_e parser_mpeg2ts::waiting_for_sync()
{
    parser_mpeg2ts_status_e status = parser_mpeg2ts_status_e::NOT_SYNCHRONIZED;

    for (std::size_t n = 0; n < mpeg2ts_parser_packet_sizes_elements; ++n) {
        int offset;
        std::size_t packet_size = mpeg2ts_parser_packet_sizes[n];

        if (m_flatbuffer.read_available() < (packet_size * (PARSER_MPEG2TS_FORWARD_SCAN + 1))) {
            status = parser_mpeg2ts_status_e::NEED_BYTES;
            break;
        }

        offset = synchronize(packet_size);
        if (PARSER_MPEG2TS_NO_SYNC_INDICATOR != offset) {
            m_flatbuffer.consume(offset);
            m_state_function = &parser_mpeg2ts::synchronized;
            m_tspacket_size = packet_size;
            status = parser_mpeg2ts_status_e::SYNC_GAINED;
            break;
        }
    }

    if (parser_mpeg2ts_status_e::NOT_SYNCHRONIZED == status)
        m_flatbuffer.consume(PARSER_MPEG2TS_PACKET_SIZE);

    return status;
}

parser_mpeg2ts_status_e parser_mpeg2ts::synchronized()
{
    if (m_flatbuffer.read_available() < m_tspacket_size)
        return parser_mpeg2ts_status_e::NEED_BYTES;

    parser_mpeg2ts_status_e status = parser_mpeg2ts_status_e::SYNCHRONIZED;

    const uint8_t *tsp = m_flatbuffer.read_ptr();
    if (PARSER_MPEG2TS_SYNC_BYTE == *tsp) {
        if (!m_transport_rate.is_valid())
            if (m_transport_rate.calculate(tsp))
                status = parser_mpeg2ts_status_e::TRANSPORT_RATE_DETECTED;
    }
    else {
        m_state_function = &parser_mpeg2ts::waiting_for_sync;
        m_tspacket_size = 0;
        m_transport_rate.reset();
        status = parser_mpeg2ts_status_e::SYNC_LOST;
    }

    return status;
}

bool parser_mpeg2ts::transport_rate::calculate(const uint8_t* tsp)
{
    bool status = false;

    if (0 == m_transport_rate) {
        mpeg2ts_pid pid = parser_mpeg2ts_get_pid(tsp);

        if (MPEG2TS_PID_INVALID == m_pcr_pid) {
           if (parser_mpeg2ts_has_pcr(tsp)) { /* 1st pcr */
              m_pcr1 = parser_mpeg2ts_get_pcr(tsp);
              m_pcr_pid = pid;
           }
        }
        else {
           m_no_of_packets++;
           if (pid == m_pcr_pid) {
              if (parser_mpeg2ts_has_pcr(tsp)) { /* 2nd pcr */
                 m_pcr2 = parser_mpeg2ts_get_pcr(tsp);
                 if (m_pcr2 > m_pcr1) {
                     m_transport_rate = m_no_of_packets * 27000000 /
                         (m_pcr2 - m_pcr1);
                     status = true;
                 }
                 else {
                     m_pcr1 = m_pcr2;
                     m_no_of_packets = 0;
                 }
              }
           }
        }
    }
    else
        status = true;

    return status;
}

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

