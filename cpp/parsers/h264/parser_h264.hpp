/**
 * @file parser_h264.hpp
 *
 * Definition of H.264 (ISO/IEC 14496-10) parser.
 *
 * H.264 syntax and semantics is defined in ISO/IEC 14496-10
 * "Information technology — Coding of audio-visual objects —
 * Part 10: Advanced Video Coding"
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

#ifndef _PARSER_H264_HPP_
#define _PARSER_H264_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <sstream>
#include <cstring>
#include <functional> /* for std::function */

#if defined(DEBUG)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "../../istream/istream.hpp"
#include "../base/parser_base.hpp"

#include "h264_structure.hpp"
#include "aud.hpp"
#include "sps.hpp"
#include "pps.hpp"
#include "sei.hpp"
#include "slice_header.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define PARSER_H264_CONTAINERS \
    PARSER_H264_CONTAINER(NONE) \
    PARSER_H264_CONTAINER(ANNEX_B) \

#define PARSER_H264_STATUSES \
    PARSER_H264_STATUS(NEED_BYTES) \
    PARSER_H264_STATUS(NAL_UNIT_SKIPPED) \
    PARSER_H264_STATUS(NAL_UNIT_CORRUPTED) \
    PARSER_H264_STATUS(AUD_PARSED) \
    PARSER_H264_STATUS(SPS_PARSED) \
    PARSER_H264_STATUS(PPS_PARSED) \
    PARSER_H264_STATUS(SEI_PARSED) \
    PARSER_H264_STATUS(SLICE_PARSED) \

#define PARSER_H264_STRUCTURES \
    PARSER_H264_STRUCTURE(AUD) \
    PARSER_H264_STRUCTURE(SPS) \
    PARSER_H264_STRUCTURE(PPS) \
    PARSER_H264_STRUCTURE(SEI) \
    PARSER_H264_STRUCTURE(SLICE_HEADER) \
    PARSER_H264_STRUCTURE(SLICE_DATA) \

#define H264_PARSER_MAX_NUMBER_OF_SPS          32
#define H264_PARSER_MAX_NUMBER_OF_PPS         256
#define H264_PARSER_STRUCTURE_ID_RECENT       (-1)
#define H264_PARSER_STRUCTURE_ID_PREVIOUS     (-2)

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

enum class parser_h264_container_e : int32_t
{
#define PARSER_H264_CONTAINER(id) id,
    PARSER_H264_CONTAINERS
#undef PARSER_H264_CONTAINER
};

enum class parser_h264_status_e : int32_t
{
#define PARSER_H264_STATUS(id) id,
    PARSER_H264_STATUSES
#undef PARSER_H264_STATUS
};

enum class parser_h264_structure_e : int32_t
{
#define PARSER_H264_STRUCTURE(id) id,
    PARSER_H264_STRUCTURES
#undef PARSER_H264_STRUCTURE
};

class parser_h264 : public parser_base<uint8_t>
{
public:
    explicit parser_h264(std::size_t capacity, parser_h264_container_e container) :
        parser_base<uint8_t>{capacity},
        m_parse_function{nullptr},
        m_aud{},
        m_sps_table{},
        m_recent_sps{-1},
        m_pps_table{},
        m_recent_pps{-1},
        m_sei{},
        m_slice_header{}
    {
        switch (container) {
            case parser_h264_container_e::NONE:
                m_parse_function = &parser_h264::parse_nal_units;
                break;

            case parser_h264_container_e::ANNEX_B:
                m_parse_function = &parser_h264::parse_byte_stream_nal_units;
                break;

            default:
                /* do nothing */
                break;
        }

#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    virtual ~parser_h264()
    {
#if defined(DEBUG)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    parser_h264(const parser_h264&) = delete;
    parser_h264(parser_h264&&) = delete;
    parser_h264& operator = (const parser_h264&) = delete;
    parser_h264& operator = (parser_h264&&) = delete;

    void reset()
    {
        parser_base::reset();
    }

    std::string to_string() const
    {
        std::ostringstream stream;

        stream << parser_base::to_string();
        stream << std::endl;
        stream << "parser_h264@";
        stream << std::hex << this;

        return stream.str();
    }

    operator std::string () const
    {
        return to_string();
    }

    parser_h264_status_e parse()
    {
        return (this->*m_parse_function)();
    }

    /**
     * Gives pointer to the requested structure.
     *
     * @param[in] structure The type of the requested structure.
     * @param[in] id Additional identifier of the structure.
     *               Valid only for sps, pps and slice headers.
     *               If H264_PARSER_STRUCTURE_ID_RECENT is passed,
     *               then recently parsed structure is provided in return value.
     *               If H264_PARSER_STRUCTURE_ID_PREVIOUS is passed
     *               (valid only for slice headers),
     *               then previously (the one before recent) parsed structure
     *               is provided in return value.
     *
     * @return Pointer to the requested struture. You must cast it to proper type.
     */
    const h264_structure* get_structure(parser_h264_structure_e structure, int id);

private:
    using parse_function_t = parser_h264_status_e (parser_h264::*)();

    /**
     * Finds 'next_start_code (0x000001)' pattern in parser's flatbuffer.
     *
     * @return true when next_start_code has been found in parser's flatbuffer,
     *         false otherwise.
     *
     * @note Annex B Byte stream format
     */
    bool find_next_start_code();

    /**
     * Finds next NAL (Network Abstraction Layer) unit in parser's flatbuffer.
     *
     * @param[in,out] parser Handle to the parser's instance.
     *
     * @return Number of bytes composing nal unit (NumBytesInNALunit),
     *         0 if start of a nal unit could not be found,
     *        -1 if end of a nal unit could not be found.
     */
    int find_nal_unit();

    void parse_scaling_list_4x4(istream_be& s, h264::scaling_list_4x4 (&lists)[SL_4x4_NUM], int list);
    void parse_scaling_list_8x8(istream_be& s, h264::scaling_list_8x8 (&lists)[SL_8x8_NUM], int list);
    void parse_scaling_matrices_4x4(istream_be& s, h264::scaling_matrices& sm);
    void parse_scaling_matrices_8x8(istream_be& s, h264::scaling_matrices& sm, uint32_t chroma_format_idc);
    bool parse_scaling_matrices(istream_be& s, h264::scaling_matrices& sm, bool parse8x8, uint32_t chroma_format_idc);

    bool parse_hrd_parameters(istream_be& s, h264::hrd_parameters& hrd);
    bool parse_vui_parameters(istream_be& s, h264::vui_parameters& vui);

    bool parse_ref_pic_list_modification(istream_be& s, h264::ref_pic_list_modification& rplm);
    bool parse_pred_weight_table(istream_be& s, h264::pred_weight_table& pwt);
    bool parse_dec_ref_pic_marking_idr(istream_be& s, h264::dec_ref_pic_marking_idr& drpm);
    bool parse_dec_ref_pic_marking_nonidr(istream_be& s, h264::dec_ref_pic_marking_nonidr& drpm);

    parser_h264_status_e parse_aud(istream_be& s);
    parser_h264_status_e parse_sps(istream_be& s);
    parser_h264_status_e parse_pps(istream_be& s);
    parser_h264_status_e parse_sei(istream_be& s);
    parser_h264_status_e parse_slice_header(istream_be& s, uint32_t nal_ref_idc, uint32_t nal_unit_type);
    parser_h264_status_e parse_slice_data(istream_be& s);
    parser_h264_status_e parse_nal_unit(istream_be& s);

    parser_h264_status_e parse_nal_units();
    parser_h264_status_e parse_byte_stream_nal_units();

    parse_function_t m_parse_function;

    h264::aud m_aud;

    h264::sps  m_sps_table[H264_PARSER_MAX_NUMBER_OF_SPS];
    int        m_recent_sps;

    h264::pps  m_pps_table[H264_PARSER_MAX_NUMBER_OF_PPS];
    int        m_recent_pps;

    h264::sei m_sei;

    h264::slice_header m_slice_header;
};

} /* end of namespace ymn */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace ymn
{

constexpr const char* to_string(parser_h264_container_e e)
{
    const char *str = "invalid 'parser_h264_container_e' value";

    switch (e) {
#define PARSER_H264_CONTAINER(id) case parser_h264_container_e::id: str = #id; break;
            PARSER_H264_CONTAINERS
#undef PARSER_H264_CONTAINER
    }

    return str;
}

constexpr const char* to_string(parser_h264_status_e e)
{
    const char *str = "invalid 'parser_status_e' value";

    switch (e) {
#define PARSER_H264_STATUS(id) case parser_h264_status_e::id: str = #id; break;
            PARSER_H264_STATUSES
#undef PARSER_H264_STATUS
    }

    return str;
}

constexpr const char* to_string(parser_h264_structure_e e)
{
    const char *str = "invalid 'parser_h264_structure_e' value";

    switch (e) {
#define PARSER_H264_STRUCTURE(id) case parser_h264_structure_e::id: str = #id; break;
            PARSER_H264_STRUCTURES
#undef PARSER_H264_STRUCTURE
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

#endif /* _PARSER_H264_HPP_ */
