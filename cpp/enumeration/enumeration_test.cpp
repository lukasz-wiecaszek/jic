/**
 * @file enumeration_test.cpp
 *
 * Test procedures for 'enumeration' type.
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
#include "gtest/gtest.h"
#include "card_colour.hpp"

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
/* Because the noexcept operator always returns true for a constant expression,
it can be used to check if a particular invocation of a constexpr
takes the constant expression branch. */
#define IS_CONSTEXPR(expr) noexcept(expr)
//#define IS_CONSTEXPR(expr) __builtin_constant_p(expr)

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

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

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
namespace
{

TEST(enumeration, is_constructor_constexpr_1)
{
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(ymn::card_colour_e::HEARTS)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(ymn::card_colour_e::DIAMONDS)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(ymn::card_colour_e::CLUBS)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(ymn::card_colour_e::SPADES)));
}

TEST(enumeration, is_constructor_constexpr_2)
{
    const ymn::card_colour_e hearts = ymn::card_colour_e::HEARTS;
    const ymn::card_colour_e diamonds = ymn::card_colour_e::DIAMONDS;
    constexpr ymn::card_colour_e clubs = ymn::card_colour_e::CLUBS;
    constexpr ymn::card_colour_e spades = ymn::card_colour_e::SPADES;

    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(diamonds)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(clubs)));
    EXPECT_TRUE(IS_CONSTEXPR(ymn::card_colour(spades)));
}

TEST(enumeration, is_constructor_constexpr_3)
{
    ymn::card_colour_e hearts = ymn::card_colour_e::HEARTS;
    ymn::card_colour_e diamonds = ymn::card_colour_e::DIAMONDS;
    ymn::card_colour_e clubs = ymn::card_colour_e::CLUBS;
    ymn::card_colour_e spades = ymn::card_colour_e::SPADES;

    EXPECT_FALSE(IS_CONSTEXPR(ymn::card_colour(hearts)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::card_colour(diamonds)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::card_colour(clubs)));
    EXPECT_FALSE(IS_CONSTEXPR(ymn::card_colour(spades)));
}

TEST(enumeration, are_cast_operators_constexpr)
{
    constexpr ymn::card_colour hearts = ymn::card_colour_e::HEARTS;

    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator const ymn::card_colour_e()));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator const typename std::underlying_type<ymn::card_colour_e>::type()));
}

TEST(enumeration, are_comparition_operators_constexpr)
{
    constexpr ymn::card_colour hearts = ymn::card_colour_e::HEARTS;

    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator == (hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator != (hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator >= (hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator >= (hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator <  (hearts)));
    EXPECT_TRUE(IS_CONSTEXPR(hearts.operator <= (hearts)));
}

TEST(enumeration, create_on_stack)
{
    const ymn::card_colour hearts = ymn::card_colour_e::HEARTS;
    const ymn::card_colour diamonds = ymn::card_colour_e::DIAMONDS;
    const ymn::card_colour clubs = ymn::card_colour_e::CLUBS;
    const ymn::card_colour spades = ymn::card_colour_e::SPADES;

    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::HEARTS), ymn::to_string(hearts));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::DIAMONDS), ymn::to_string(diamonds));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::CLUBS), ymn::to_string(clubs));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::SPADES), ymn::to_string(spades));
}

TEST(enumeration, create_on_heap)
{
    const ymn::card_colour* hearts = new ymn::card_colour(ymn::card_colour_e::HEARTS);
    const ymn::card_colour* diamonds = new ymn::card_colour(ymn::card_colour_e::DIAMONDS);
    const ymn::card_colour* clubs = new ymn::card_colour(ymn::card_colour_e::CLUBS);
    const ymn::card_colour* spades = new ymn::card_colour(ymn::card_colour_e::SPADES);

    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::HEARTS), ymn::to_string(*hearts));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::DIAMONDS), ymn::to_string(*diamonds));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::CLUBS), ymn::to_string(*clubs));
    EXPECT_EQ(ymn::to_string(ymn::card_colour_e::SPADES), ymn::to_string(*spades));

    delete hearts;
    delete diamonds;
    delete clubs;
    delete spades;
}

TEST(enumeration, cast_operators)
{
    const ymn::card_colour hearts = ymn::card_colour_e::HEARTS;
    EXPECT_EQ(ymn::card_colour_e::HEARTS, hearts);
    EXPECT_EQ(0, hearts);

    const ymn::card_colour diamonds = ymn::card_colour_e::DIAMONDS;
    EXPECT_EQ(ymn::card_colour_e::DIAMONDS, diamonds);
    EXPECT_EQ(1, diamonds);

    const ymn::card_colour clubs = ymn::card_colour_e::CLUBS;
    EXPECT_EQ(ymn::card_colour_e::CLUBS, clubs);
    EXPECT_EQ(2, clubs);

    const ymn::card_colour spades = ymn::card_colour_e::SPADES;
    EXPECT_EQ(ymn::card_colour_e::SPADES, spades);
    EXPECT_EQ(3, spades);
}

TEST(enumeration, comparition_operators)
{
    const ymn::card_colour hearts = ymn::card_colour_e::HEARTS;
    const ymn::card_colour diamonds = ymn::card_colour_e::DIAMONDS;
    const ymn::card_colour clubs = ymn::card_colour_e::CLUBS;
    const ymn::card_colour spades = ymn::card_colour_e::SPADES;

    EXPECT_TRUE(hearts == hearts);
    EXPECT_FALSE(hearts != hearts);

    EXPECT_TRUE(hearts != diamonds);
    EXPECT_FALSE(hearts == diamonds);

    EXPECT_TRUE(spades > clubs);
    EXPECT_FALSE(spades <= clubs);

    EXPECT_TRUE(spades >= spades);
    EXPECT_FALSE(spades < spades);

    EXPECT_TRUE(clubs < spades);
    EXPECT_FALSE(clubs >= spades);

    EXPECT_TRUE(clubs <= clubs);
    EXPECT_FALSE(clubs > clubs);
}

} // end of anonymous namespace

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/
