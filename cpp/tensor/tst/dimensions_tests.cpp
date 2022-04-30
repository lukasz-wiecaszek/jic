/* SPDX-License-Identifier: MIT */
/**
 * @file dimensions_tests.cpp
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <type_traits>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <dimensions.hpp>
#include <gtest/gtest.h>

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/

/*===========================================================================*\
 * global (external linkage) object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local (internal linkage) function declarations
\*===========================================================================*/

/*===========================================================================*\
 * local (internal linkage) object definitions
\*===========================================================================*/

/*===========================================================================*\
 * static inline (internal linkage) function definitions
\*===========================================================================*/

/*===========================================================================*\
 * global (external linkage) function definitions
\*===========================================================================*/
TEST(dimensions, order_0)
{
    lts::dimensions<> dim;

    EXPECT_EQ(0, dim.order);
    EXPECT_EQ(1, dim.size);

    int i = 0;
    dim.for_each([&](const decltype(dim)::array& indices) {
        i++;
    });

    EXPECT_EQ(1, i);
}

TEST(dimensions, order_1)
{
    const int d1 = 3;
    lts::dimensions<d1> dim;

    EXPECT_EQ(1, dim.order);
    EXPECT_EQ(d1, dim.size);

    EXPECT_EQ(d1, dim.d[0]);
    EXPECT_EQ(d1, dim.n[0]);
    EXPECT_EQ(1,  dim.o[0]);

    const std::vector<decltype(dim)::array> expected_indices = {
        {0}, {1}, {2}
    };

    int i = 0;
    dim.for_each([&](const decltype(dim)::array& indices) {
        EXPECT_TRUE((std::equal(std::begin(indices), std::end(indices), std::begin(expected_indices[i++]))));
    });

    EXPECT_EQ(d1, i);
}

TEST(dimensions, order_2)
{
    const int d1 = 3;
    const int d2 = 5;
    lts::dimensions<d1, d2> dim;

    EXPECT_EQ(2, dim.order);
    EXPECT_EQ(d1 * d2, dim.size);

    EXPECT_EQ(d1,      dim.d[0]);
    EXPECT_EQ(d2,      dim.d[1]);

    EXPECT_EQ(d1,      dim.n[0]);
    EXPECT_EQ(d1 * d2, dim.n[1]);

    EXPECT_EQ(d2,      dim.o[0]);
    EXPECT_EQ(1,       dim.o[1]);

    const std::vector<decltype(dim)::array> expected_indices = {
        {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},
        {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4},
        {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4},
    };

    int i = 0;
    dim.for_each([&](const decltype(dim)::array& indices) {
        EXPECT_TRUE((std::equal(std::begin(indices), std::end(indices), std::begin(expected_indices[i++]))));
    });

    EXPECT_EQ(d1 * d2, i);
}

TEST(dimensions, order_3)
{
    const int d1 = 3;
    const int d2 = 5;
    const int d3 = 7;
    lts::dimensions<d1, d2, d3> dim;

    EXPECT_EQ(3, dim.order);
    EXPECT_EQ(d1 * d2 * d3, dim.size);

    EXPECT_EQ(d1,           dim.d[0]);
    EXPECT_EQ(d2,           dim.d[1]);
    EXPECT_EQ(d3,           dim.d[2]);

    EXPECT_EQ(d1,           dim.n[0]);
    EXPECT_EQ(d1 * d2,      dim.n[1]);
    EXPECT_EQ(d1 * d2 * d3, dim.n[2]);

    EXPECT_EQ(d2 * d3,      dim.o[0]);
    EXPECT_EQ(d3,           dim.o[1]);
    EXPECT_EQ(1,            dim.o[2]);

    const std::vector<decltype(dim)::array> expected_indices = {
        {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3}, {0, 0, 4}, {0, 0, 5}, {0, 0, 6},
        {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 1, 5}, {0, 1, 6},
        {0, 2, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 2, 6},
        {0, 3, 0}, {0, 3, 1}, {0, 3, 2}, {0, 3, 3}, {0, 3, 4}, {0, 3, 5}, {0, 3, 6},
        {0, 4, 0}, {0, 4, 1}, {0, 4, 2}, {0, 4, 3}, {0, 4, 4}, {0, 4, 5}, {0, 4, 6},
        {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 0, 3}, {1, 0, 4}, {1, 0, 5}, {1, 0, 6},
        {1, 1, 0}, {1, 1, 1}, {1, 1, 2}, {1, 1, 3}, {1, 1, 4}, {1, 1, 5}, {1, 1, 6},
        {1, 2, 0}, {1, 2, 1}, {1, 2, 2}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 2, 6},
        {1, 3, 0}, {1, 3, 1}, {1, 3, 2}, {1, 3, 3}, {1, 3, 4}, {1, 3, 5}, {1, 3, 6},
        {1, 4, 0}, {1, 4, 1}, {1, 4, 2}, {1, 4, 3}, {1, 4, 4}, {1, 4, 5}, {1, 4, 6},
        {2, 0, 0}, {2, 0, 1}, {2, 0, 2}, {2, 0, 3}, {2, 0, 4}, {2, 0, 5}, {2, 0, 6},
        {2, 1, 0}, {2, 1, 1}, {2, 1, 2}, {2, 1, 3}, {2, 1, 4}, {2, 1, 5}, {2, 1, 6},
        {2, 2, 0}, {2, 2, 1}, {2, 2, 2}, {2, 2, 3}, {2, 2, 4}, {2, 2, 5}, {2, 2, 6},
        {2, 3, 0}, {2, 3, 1}, {2, 3, 2}, {2, 3, 3}, {2, 3, 4}, {2, 3, 5}, {2, 3, 6},
        {2, 4, 0}, {2, 4, 1}, {2, 4, 2}, {2, 4, 3}, {2, 4, 4}, {2, 4, 5}, {2, 4, 6},
    };

    int i = 0;
    dim.for_each([&](const decltype(dim)::array& indices) {
        EXPECT_TRUE((std::equal(std::begin(indices), std::end(indices), std::begin(expected_indices[i++]))));
    });

    EXPECT_EQ(d1 * d2 * d3, i);
}

TEST(dimensions, subset)
{
    const int d1 = 3;
    const int d2 = 5;
    const int d3 = 7;
    const int d4 = 42;
    lts::dimensions<d1, d2, d3, d4> dim1;

    EXPECT_EQ(4, dim1.order);
    EXPECT_EQ(d1 * d2 * d3 * d4, dim1.size);

    EXPECT_EQ(d1, dim1.d[0]);
    EXPECT_EQ(d2, dim1.d[1]);
    EXPECT_EQ(d3, dim1.d[2]);
    EXPECT_EQ(d4, dim1.d[3]);

    lts::dimensions_subset<decltype(dim1), std::index_sequence<0, 2>>::type dim2;

    EXPECT_EQ(2, dim2.order);
    EXPECT_EQ(d1 * d3, dim2.size);

    EXPECT_EQ(d1, dim2.d[0]);
    EXPECT_EQ(d3, dim2.d[1]);

    lts::dimensions_subset<decltype(dim1), std::index_sequence<1, 3>>::type dim3;

    EXPECT_EQ(2, dim3.order);
    EXPECT_EQ(d2 * d4, dim3.size);

    EXPECT_EQ(d2, dim3.d[0]);
    EXPECT_EQ(d4, dim3.d[1]);
}

TEST(dimensions, dimensions_head)
{
    const int d1 = 3;
    const int d2 = 5;
    const int d3 = 7;
    const int d4 = 42;
    lts::dimensions<d1, d2, d3, d4> dim1;

    EXPECT_EQ(4, dim1.order);
    EXPECT_EQ(d1 * d2 * d3 * d4, dim1.size);

    EXPECT_EQ(d1, dim1.d[0]);
    EXPECT_EQ(d2, dim1.d[1]);
    EXPECT_EQ(d3, dim1.d[2]);
    EXPECT_EQ(d4, dim1.d[3]);

    lts::dimensions_head<decltype(dim1), 3> dim2;

    EXPECT_EQ(3, dim2.order);
    EXPECT_EQ(d1 * d2 * d3, dim2.size);

    EXPECT_EQ(d1, dim2.d[0]);
    EXPECT_EQ(d2, dim2.d[1]);
    EXPECT_EQ(d3, dim2.d[2]);

    lts::dimensions_head<decltype(dim2), 2> dim3;

    EXPECT_EQ(2, dim3.order);
    EXPECT_EQ(d1 * d2, dim3.size);

    EXPECT_EQ(d1, dim3.d[0]);
    EXPECT_EQ(d2, dim3.d[1]);

    lts::dimensions_head<decltype(dim1), 2> dim4;

    EXPECT_TRUE((std::is_same<decltype(dim3), decltype(dim4)>::value));
}

TEST(dimensions, dimensions_tail)
{
    const int d1 = 3;
    const int d2 = 5;
    const int d3 = 7;
    const int d4 = 42;
    lts::dimensions<d1, d2, d3, d4> dim1;

    EXPECT_EQ(4, dim1.order);
    EXPECT_EQ(d1 * d2 * d3 * d4, dim1.size);

    EXPECT_EQ(d1, dim1.d[0]);
    EXPECT_EQ(d2, dim1.d[1]);
    EXPECT_EQ(d3, dim1.d[2]);
    EXPECT_EQ(d4, dim1.d[3]);

    lts::dimensions_tail<decltype(dim1), 3> dim2;

    EXPECT_EQ(3, dim2.order);
    EXPECT_EQ(d2 * d3 * d4, dim2.size);

    EXPECT_EQ(d2, dim2.d[0]);
    EXPECT_EQ(d3, dim2.d[1]);
    EXPECT_EQ(d4, dim2.d[2]);

    lts::dimensions_tail<decltype(dim2), 2> dim3;

    EXPECT_EQ(2, dim3.order);
    EXPECT_EQ(d3 * d4, dim3.size);

    EXPECT_EQ(d3, dim3.d[0]);
    EXPECT_EQ(d4, dim3.d[1]);

    lts::dimensions_tail<decltype(dim1), 2> dim4;

    EXPECT_TRUE((std::is_same<decltype(dim3), decltype(dim4)>::value));
}

TEST(dimensions, dimensions_cat)
{
    const int d1 = 3;
    const int d2 = 5;
    const int d3 = 7;
    const int d4 = 42;
    lts::dimensions<d1, d2> dim1;
    lts::dimensions<d3, d4> dim2;

    EXPECT_EQ(2, dim1.order);
    EXPECT_EQ(d1 * d2, dim1.size);
    EXPECT_EQ(d1, dim1.d[0]);
    EXPECT_EQ(d2, dim1.d[1]);

    EXPECT_EQ(2, dim2.order);
    EXPECT_EQ(d3 * d4, dim2.size);
    EXPECT_EQ(d3, dim2.d[0]);
    EXPECT_EQ(d4, dim2.d[1]);

    lts::dimensions_cat<decltype(dim1), decltype(dim2)> dim3;

    EXPECT_EQ(4, dim3.order);
    EXPECT_EQ(d1 * d2 * d3 * d4, dim3.size);

    EXPECT_EQ(d1, dim3.d[0]);
    EXPECT_EQ(d2, dim3.d[1]);
    EXPECT_EQ(d3, dim3.d[2]);
    EXPECT_EQ(d4, dim3.d[3]);

    lts::dimensions_cat<decltype(dim2), decltype(dim1)> dim4;

    EXPECT_EQ(4, dim4.order);
    EXPECT_EQ(d1 * d2 * d3 * d4, dim4.size);

    EXPECT_EQ(d3, dim4.d[0]);
    EXPECT_EQ(d4, dim4.d[1]);
    EXPECT_EQ(d1, dim4.d[2]);
    EXPECT_EQ(d2, dim4.d[3]);
}

int main(int argc, char* argv[])
{
    int retval = EXIT_FAILURE;

    do {
        ::testing::InitGoogleTest(&argc, argv);

        retval = RUN_ALL_TESTS();
    } while (0);

    return retval;
}

/*===========================================================================*\
 * local (internal linkage) function definitions
\*===========================================================================*/

