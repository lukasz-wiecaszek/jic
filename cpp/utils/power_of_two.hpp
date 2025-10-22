/* SPDX-License-Identifier: MIT */
/**
 * @file power_of_two.hpp
 *
 * Utility function for checking if a number is a power of two.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _POWER_OF_TWO_HPP_
#define _POWER_OF_TWO_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <cstddef>

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

constexpr bool is_power_of_two(std::size_t x)
{
    return ((x & (x - 1)) == 0);
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

#endif /* _POWER_OF_TWO_HPP_ */
