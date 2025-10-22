/* SPDX-License-Identifier: MIT */
/**
 * @file structure.hpp
 *
 * XXX
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _STRUCTURE_HPP_
#define _STRUCTURE_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <tuple>

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

template<typename... TYPES>
struct structure {
    std::tuple<TYPES...> members;
};

template<int, typename, typename>
struct structure_reader;

template<
    int INDEX,
    typename STREAM,
    template<typename...> typename TUPLE,
    typename... TYPES>
struct structure_reader<INDEX, STREAM, TUPLE<TYPES...>> {
    bool operator()(STREAM& stream, TUPLE<TYPES...>& value)
    {
        structure_reader<INDEX-1, STREAM, TUPLE<TYPES...>>{}(stream, value);
        return stream >> std::get<INDEX>(value.members);
    }
};

template<
    typename STREAM,
    template<typename...> typename TUPLE,
    typename... TYPES>
struct structure_reader<0, STREAM, TUPLE<TYPES...>> {
    bool operator()(STREAM& stream, TUPLE<TYPES...>& value)
    {
        return stream >> std::get<0>(value.members);
    }
};

template<int, typename, typename>
struct structure_writer;

template<
    int INDEX,
    typename STREAM,
    template<typename...> typename TUPLE,
    typename... TYPES>
struct structure_writer<INDEX, STREAM, TUPLE<TYPES...>> {
    bool operator()(STREAM& stream, const TUPLE<TYPES...>& value)
    {
        structure_writer<INDEX-1, STREAM, TUPLE<TYPES...>>{}(stream, value);
        return stream << std::get<INDEX>(value.members);
    }
};

template<
    typename STREAM,
    template<typename...> typename TUPLE,
    typename... TYPES>
struct structure_writer<0, STREAM, TUPLE<TYPES...>> {
    bool operator()(STREAM& stream, const TUPLE<TYPES...>& value)
    {
        return stream << std::get<0>(value.members);
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

#endif /* _STRUCTURE_HPP_ */
