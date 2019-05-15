/**
 * @file scaling_matrices.hpp
 *
 * Definition of ISO/IEC 14496-10/ITU-T H.264 scaling matrices.
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

#ifndef _SCALING_MATRICES_HPP_
#define _SCALING_MATRICES_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <string>
#include <sstream>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "h264_structure.hpp"

#include "../../utils/utilities.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define SCALING_LIST_INTRA_Y  0
#define SCALING_LIST_INTRA_Cb 1
#define SCALING_LIST_INTRA_Cr 2
#define SCALING_LIST_INTER_Y  3
#define SCALING_LIST_INTER_Cb 4
#define SCALING_LIST_INTER_Cr 5
#define SCALING_LIST_NUM      6

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{
namespace h264
{

struct scaling_list_4x4 : public h264_structure
{
    scaling_list_4x4() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        memset(scaling_list, 16, sizeof(scaling_list));
    }

    std::string to_string() const override;

    uint32_t scaling_list_present_flag;
    uint8_t  scaling_list[16];
};

inline std::string scaling_list_4x4::to_string() const
{
    std::ostringstream stream;

    stream << "  scaling_list_4x4 - to be implemented" << std::endl;

    return stream.str();
}

struct scaling_list_8x8 : public h264_structure
{
    scaling_list_8x8() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        memset(scaling_list, 16, sizeof(scaling_list));
    }

    std::string to_string() const override;

    uint32_t scaling_list_present_flag;
    uint8_t  scaling_list[64];
};

inline std::string scaling_list_8x8::to_string() const
{
    std::ostringstream stream;

    stream << "  scaling_list_8x8 - to be implemented" << std::endl;

    return stream.str();
}

struct scaling_matrices : public h264_structure
{
    scaling_matrices() :
        h264_structure()
    {
    }

    void set_defaults()
    {
        for (std::size_t i = 0; i < TABLE_ELEMENTS(scaling_matrices_4x4); ++i)
            scaling_matrices_4x4[i].set_defaults();

        for (std::size_t i = 0; i < TABLE_ELEMENTS(scaling_matrices_8x8); ++i)
            scaling_matrices_8x8[i].set_defaults();
    }

    std::string to_string() const override;

    scaling_list_4x4 scaling_matrices_4x4[SCALING_LIST_NUM];
    scaling_list_8x8 scaling_matrices_8x8[SCALING_LIST_NUM];
};

inline std::string scaling_matrices::to_string() const
{
    std::ostringstream stream;

    stream << "  scaling_matrices:" << std::endl;

    stream << "    scaling matrix 4x4 intra Y:" << std::endl;
    stream << "    scaling matrix 4x4 intra Cb:" << std::endl;
    stream << "    scaling matrix 4x4 intra Cr:" << std::endl;
    stream << "    scaling matrix 4x4 inter Y:" << std::endl;
    stream << "    scaling matrix 4x4 inter Cb:" << std::endl;
    stream << "    scaling matrix 4x4 inter Cr:" << std::endl;

    stream << "    scaling matrix 8x8 intra Y:" << std::endl;
    stream << "    scaling matrix 8x8 intra Cb:" << std::endl;
    stream << "    scaling matrix 8x8 intra Cr:" << std::endl;
    stream << "    scaling matrix 8x8 inter Y:" << std::endl;
    stream << "    scaling matrix 8x8 inter Cb:" << std::endl;
    stream << "    scaling matrix 8x8 inter Cr:" << std::endl;

    return stream.str();
}

} /* end of namespace h264 */
} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
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

#endif /* _SCALING_MATRICES_HPP_ */
