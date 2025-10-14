/**
 * @file card_colour.hpp
 *
 * Definition of a 'card_colour' enumeration type.
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

#ifndef _CARD_COLOUR_HPP_
#define _CARD_COLOUR_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#if defined(DEBUG_ENUMERATION)
#include <iostream>
#endif

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include "enumeration.hpp"

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define CARD_COLOURS \
    CARD_COLOUR(HEARTS) \
    CARD_COLOUR(DIAMONDS) \
    CARD_COLOUR(CLUBS) \
    CARD_COLOUR(SPADES) \

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace lts
{

enum class card_colour_e : int32_t
{
#define CARD_COLOUR(id) id,
    CARD_COLOURS
#undef CARD_COLOUR
};

class card_colour : public enumeration<card_colour_e>
{
public:
    constexpr card_colour(const card_colour_e& e) :
        enumeration<card_colour_e>{e}
    {
#if defined(DEBUG_ENUMERATION)
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << to_string() << std::endl;
#endif
    }

    constexpr const char* to_string() const;
};

} /* end of namespace lts */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace lts
{

constexpr const char* to_string(card_colour_e e)
{
    const char *str = "invalid 'card_colour_e' value";

    switch (e) {
#define CARD_COLOUR(id) case card_colour_e::id: str = #id; break;
        CARD_COLOURS
#undef CARD_COLOUR
    }

    return str;
}

constexpr const char* card_colour::to_string() const
{
    return lts::to_string(m_value);
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

#endif /* _CARD_COLOUR_HPP_ */
