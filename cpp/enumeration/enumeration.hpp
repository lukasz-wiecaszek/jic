/**
 * @file enumeration.hpp
 *
 * Definition of an 'enumeration' type.
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

#ifndef _ENUMERATION_HPP_
#define _ENUMERATION_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <type_traits>

#if defined(DEBUG_ENUMERATION)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/
namespace ymn
{

} /* end of namespace ymn */

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace ymn
{

template<typename T>
class enumeration
{
    static_assert(std::is_enum<T>::value, "T is not an enumeration type");
public:
    constexpr enumeration(const T& value) :
        m_value{value}
    {
#if defined(DEBUG_ENUMERATION)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << static_cast<typename std::underlying_type<T>::type>(m_value) << std::endl;
#endif
    }

    constexpr operator const T () const
    {
        return m_value;
    }

    constexpr operator const typename std::underlying_type<T>::type () const
    {
        return static_cast<typename std::underlying_type<T>::type>(m_value);
    }

    constexpr bool operator == (const enumeration& e) const { return (m_value == e.m_value); }
    constexpr bool operator != (const enumeration& e) const { return !((*this) == e); }

    constexpr bool operator >  (const enumeration& e) const { return (m_value >  e.m_value); }
    constexpr bool operator >= (const enumeration& e) const { return (m_value >= e.m_value); }
    constexpr bool operator <  (const enumeration& e) const { return (m_value <  e.m_value); }
    constexpr bool operator <= (const enumeration& e) const { return (m_value <= e.m_value); }

protected:
    T m_value;
};

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

#endif /* _ENUMERATION_HPP_ */
