/**
 * @file errnotostr.hpp
 *
 * Converts errno number to corresponding string.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _ERRNOTOSTR_HPP_
#define _ERRNOTOSTR_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <errno.h>
#include <string.h>

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

static inline const char* errnotostr(int errnum)
{
    static char buf[128];
    return strerror_r(errnum, buf, sizeof(buf));
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

#endif /* _STRTOINTEGER_HPP_ */
