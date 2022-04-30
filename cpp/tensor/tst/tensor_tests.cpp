/* SPDX-License-Identifier: MIT */
/**
 * @file tensor_tests.cpp
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
#include <tensor.hpp>
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
TEST(tensor, order0)
{
    using tensor = lts::tensor<double, lts::dimensions<>, lts::dimensions<>>;
    tensor::value_type a1[tensor::size] = {42.0};
    tensor::value_type a2[tensor::size] = {};

    tensor t1{{a1[0]}};
    tensor t2{a2};
    tensor t3;

    EXPECT_TRUE(t1 != t2);
    EXPECT_FALSE(t1 == t2);

    EXPECT_EQ(0, t1.order);
    EXPECT_EQ(1, t1.size);

    EXPECT_EQ(0, t2.order);
    EXPECT_EQ(1, t2.size);

    EXPECT_EQ(0, t3.order);
    EXPECT_EQ(1, t3.size);

    t2.init({a2[0]});
    EXPECT_TRUE(t1 != t2);
    EXPECT_FALSE(t1 == t2);

    t2.init(a1);
    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 != t2);

    t1 += t2;
    EXPECT_TRUE(t1 == (2.0 * t2));

    t1 -= t2;
    EXPECT_TRUE(t1 == t2);

    t1 *= 3;
    EXPECT_TRUE(t1 == (3.0 * t2));

    t1 /= 3;
    EXPECT_TRUE(t1 == t2);

    t3 = t1 + t2;
    EXPECT_TRUE((t1 + t2) == t3);
    EXPECT_TRUE((t1 + t2) == (t2 + t1));

    t3 = t1 - t2;
    EXPECT_TRUE((t1 - t2) == t3);

    t2 = 3.0 * t1;
    t3 = t1 * 3.0;
    EXPECT_TRUE(t2 == t3);

    t3 = t2 / 3.0;
    EXPECT_TRUE(t1 == t3);

    EXPECT_EQ(a1[0],     t1.get({},{}));
    EXPECT_EQ(a1[0] * 3, t2.get({},{}));
    EXPECT_EQ(a1[0],     t3.get({},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(tensor, order1)
{
    using tensor = lts::tensor<double, lts::dimensions<3>, lts::dimensions<>>;
    tensor::value_type a1[tensor::size] = {1.0, 2.0, 3.0};
    tensor::value_type a2[tensor::size] = {};

    tensor t1{{a1[0], a1[1], a1[2]}};
    tensor t2{a2};
    tensor t3;

    EXPECT_TRUE(t1 != t2);
    EXPECT_FALSE(t1 == t2);

    EXPECT_EQ(1, t1.order);
    EXPECT_EQ(3, t1.size);

    EXPECT_EQ(1, t2.order);
    EXPECT_EQ(3, t2.size);

    EXPECT_EQ(1, t3.order);
    EXPECT_EQ(3, t3.size);

    t2.init({a2[0], a2[1], a2[2]});
    EXPECT_TRUE(t1 != t2);
    EXPECT_FALSE(t1 == t2);

    t2.init(a1);
    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 != t2);

    t1 += t2;
    EXPECT_TRUE(t1 == (2.0 * t2));

    t1 -= t2;
    EXPECT_TRUE(t1 == t2);

    t1 *= 3;
    EXPECT_TRUE(t1 == (3.0 * t2));

    t1 /= 3;
    EXPECT_TRUE(t1 == t2);

    t3 = t1 + t2;
    EXPECT_TRUE((t1 + t2) == t3);
    EXPECT_TRUE((t1 + t2) == (t2 + t1));

    t3 = t1 - t2;
    EXPECT_TRUE((t1 - t2) == t3);

    t2 = 3.0 * t1;
    t3 = t1 * 3.0;
    EXPECT_TRUE(t2 == t3);

    t3 = t2 / 3.0;
    EXPECT_TRUE(t1 == t3);

    EXPECT_EQ(a1[0],     t1.get({0},{}));
    EXPECT_EQ(a1[1],     t1.get({1},{}));
    EXPECT_EQ(a1[2],     t1.get({2},{}));

    EXPECT_EQ(a1[0] * 3, t2.get({0},{}));
    EXPECT_EQ(a1[1] * 3, t2.get({1},{}));
    EXPECT_EQ(a1[2] * 3, t2.get({2},{}));

    EXPECT_EQ(a1[0],     t3.get({0},{}));
    EXPECT_EQ(a1[1],     t3.get({1},{}));
    EXPECT_EQ(a1[2],     t3.get({2},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(tensor, subtensor)
{
    using tensor = lts::tensor<double, lts::dimensions<3>, lts::dimensions<3>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}};

    EXPECT_EQ(2, t1.order);
    EXPECT_EQ(9, t1.size);

    auto subtensor = t1.subtensor(std::index_sequence<1>{});
    subtensor->init({11.0, 12.0, 13.0});

    EXPECT_EQ(1, subtensor->order);
    EXPECT_EQ(3, subtensor->size);

    std::cout << t1.to_string() << std::endl;
    std::cout << subtensor->to_string() << std::endl;
}

TEST(tensor, product1)
{
    using tensor1 = lts::tensor<double, lts::dimensions<>, lts::dimensions<3>>;
    using tensor2 = lts::tensor<double, lts::dimensions<3>, lts::dimensions<>>;

    tensor1 t1{{1.0, 2.0, 3.0}};
    tensor2 t2{{4.0, 5.0, 6.0}};

    auto t3 = t1 * t2;
    EXPECT_EQ(0, t3.order);
    EXPECT_EQ(1, t3.size);

    const int expected_value =
        t1.get({},{0}) * t2.get({0},{}) +
        t1.get({},{1}) * t2.get({1},{}) +
        t1.get({},{2}) * t2.get({2},{});
    EXPECT_EQ(expected_value, t3.get({},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(tensor, product2)
{
    using tensor1 = lts::tensor<double, lts::dimensions<>, lts::dimensions<3, 2>>;
    using tensor2 = lts::tensor<double, lts::dimensions<3, 2>, lts::dimensions<>>;

    tensor1 t1{{11.0, 12.0, 13.0, 14.0, 15.0, 16.0}};
    tensor2 t2{{21.0, 22.0, 23.0, 24.0, 25.0, 26.0}};

    auto t3 = t1 * t2;
    EXPECT_EQ(0, t3.order);
    EXPECT_EQ(1, t3.size);

    const int expected_value =
        t1.get({},{0,0}) * t2.get({0,0},{}) +
        t1.get({},{0,1}) * t2.get({0,1},{}) +
        t1.get({},{1,0}) * t2.get({1,0},{}) +
        t1.get({},{1,1}) * t2.get({1,1},{}) +
        t1.get({},{2,0}) * t2.get({2,0},{}) +
        t1.get({},{2,1}) * t2.get({2,1},{});
    EXPECT_EQ(expected_value, t3.get({},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(tensor, product3)
{
    using tensor1 = lts::tensor<double, lts::dimensions<3>, lts::dimensions<2>>;
    using tensor2 = lts::tensor<double, lts::dimensions<2>, lts::dimensions<>>;

    tensor1 t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}};
    tensor2 t2{{1.0, 2.0}};

    auto t3 = t1 * t2;
    EXPECT_EQ(1, t3.order);
    EXPECT_EQ(3, t3.size);

    const int expected_value_0 =
        t1.get({0},{0}) * t2.get({0},{}) +
        t1.get({0},{1}) * t2.get({1},{});
    const int expected_value_1 =
        t1.get({1},{0}) * t2.get({0},{}) +
        t1.get({1},{1}) * t2.get({1},{});
    const int expected_value_2 =
        t1.get({2},{0}) * t2.get({0},{}) +
        t1.get({2},{1}) * t2.get({1},{});

    EXPECT_EQ(expected_value_0, t3.get({0},{}));
    EXPECT_EQ(expected_value_1, t3.get({1},{}));
    EXPECT_EQ(expected_value_2, t3.get({2},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(test, transpose1)
{
    using tensor = lts::tensor<double, lts::dimensions<>, lts::dimensions<3>>;

    tensor t1{{1.0, 2.0, 3.0}};
    auto t2 = transpose(t1);

    EXPECT_TRUE((std::is_same<decltype(t2), lts::tensor<double, lts::dimensions<3>, lts::dimensions<>>>::value));

    auto t3 = t1 * t2;
    EXPECT_EQ(0, t3.order);
    EXPECT_EQ(1, t3.size);

    const int expected_value =
        t1.get({},{0}) * t2.get({0},{}) +
        t1.get({},{1}) * t2.get({1},{}) +
        t1.get({},{2}) * t2.get({2},{});
    EXPECT_EQ(expected_value, t3.get({},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(test, transpose2)
{
    using tensor = lts::tensor<double, lts::dimensions<>, lts::dimensions<3, 2>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}};
    auto t2 = transpose(t1);

    EXPECT_TRUE((std::is_same<decltype(t2), lts::tensor<double, lts::dimensions<3, 2>, lts::dimensions<>>>::value));

    auto t3 = t1 * t2;
    EXPECT_EQ(0, t3.order);
    EXPECT_EQ(1, t3.size);

    const int expected_value =
        t1.get({},{0,0}) * t2.get({0,0},{}) +
        t1.get({},{0,1}) * t2.get({0,1},{}) +
        t1.get({},{1,0}) * t2.get({1,0},{}) +
        t1.get({},{1,1}) * t2.get({1,1},{}) +
        t1.get({},{2,0}) * t2.get({2,0},{}) +
        t1.get({},{2,1}) * t2.get({2,1},{});
    EXPECT_EQ(expected_value, t3.get({},{}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(test, transpose3)
{
    using tensor = lts::tensor<double, lts::dimensions<2>, lts::dimensions<3, 2>>;

    tensor t1{{11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0}};
    auto t2 = transpose(t1);

    EXPECT_TRUE((std::is_same<decltype(t2), lts::tensor<double, lts::dimensions<3, 2>, lts::dimensions<2>>>::value));

    auto t3 = t1 * t2;
    EXPECT_EQ(2, t3.order);
    EXPECT_EQ(4, t3.size);

    const int expected_value_0_0 =
        t1.get({0},{0,0}) * t2.get({0,0},{0}) +
        t1.get({0},{0,1}) * t2.get({0,1},{0}) +
        t1.get({0},{1,0}) * t2.get({1,0},{0}) +
        t1.get({0},{1,1}) * t2.get({1,1},{0}) +
        t1.get({0},{2,0}) * t2.get({2,0},{0}) +
        t1.get({0},{2,1}) * t2.get({2,1},{0});
    const int expected_value_0_1 =
        t1.get({0},{0,0}) * t2.get({0,0},{1}) +
        t1.get({0},{0,1}) * t2.get({0,1},{1}) +
        t1.get({0},{1,0}) * t2.get({1,0},{1}) +
        t1.get({0},{1,1}) * t2.get({1,1},{1}) +
        t1.get({0},{2,0}) * t2.get({2,0},{1}) +
        t1.get({0},{2,1}) * t2.get({2,1},{1});
    const int expected_value_1_0 =
        t1.get({1},{0,0}) * t2.get({0,0},{0}) +
        t1.get({1},{0,1}) * t2.get({0,1},{0}) +
        t1.get({1},{1,0}) * t2.get({1,0},{0}) +
        t1.get({1},{1,1}) * t2.get({1,1},{0}) +
        t1.get({1},{2,0}) * t2.get({2,0},{0}) +
        t1.get({1},{2,1}) * t2.get({2,1},{0});
    const int expected_value_1_1 =
        t1.get({1},{0,0}) * t2.get({0,0},{1}) +
        t1.get({1},{0,1}) * t2.get({0,1},{1}) +
        t1.get({1},{1,0}) * t2.get({1,0},{1}) +
        t1.get({1},{1,1}) * t2.get({1,1},{1}) +
        t1.get({1},{2,0}) * t2.get({2,0},{1}) +
        t1.get({1},{2,1}) * t2.get({2,1},{1});

    EXPECT_EQ(expected_value_0_0, t3.get({0},{0}));
    EXPECT_EQ(expected_value_0_1, t3.get({0},{1}));
    EXPECT_EQ(expected_value_1_0, t3.get({1},{0}));
    EXPECT_EQ(expected_value_1_1, t3.get({1},{1}));

    std::cout << t1.to_string() << std::endl;
    std::cout << t2.to_string() << std::endl;
    std::cout << t3.to_string() << std::endl;
}

TEST(test, det1)
{
    using tensor = lts::tensor<double, lts::dimensions<2>, lts::dimensions<2>>;

    tensor t1{{1.0, 2.0, 3.0, 0.0}};

    EXPECT_EQ(-6, t1.det());

    std::cout << t1.to_string() << std::endl;
}

TEST(test, det2)
{
    using tensor = lts::tensor<double, lts::dimensions<3>, lts::dimensions<3>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 0.0}};

    EXPECT_EQ(27, t1.det());

    std::cout << t1.to_string() << std::endl;
}

TEST(test, det3)
{
    using tensor = lts::tensor<double, lts::dimensions<4>, lts::dimensions<4>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 0.0}};

    EXPECT_EQ(0, t1.det());

    std::cout << t1.to_string() << std::endl;
}

TEST(test, inverse1)
{
    using tensor = lts::tensor<double, lts::dimensions<2>, lts::dimensions<2>>;

    tensor t1{{1.0, 2.0, 3.0, 0.0}};
    tensor t2 = inverse(t1, t1.det());
    tensor t3{{0.0, -2.0, -3.0, +1.0}};

    t3 /= t1.det();
    EXPECT_TRUE(t2 == t3);

    std::cout << t2.to_string() << std::endl;
}

TEST(test, inverse2)
{
    using tensor = lts::tensor<double, lts::dimensions<3>, lts::dimensions<3>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 0.0}};
    tensor t2 = inverse(t1, t1.det());
    tensor t3{{-48.0, +24.0, -3.0, 42.0, -21.0, 6.0, -3.0, 6.0, -3.0}};

    t3 /= t1.det();
    EXPECT_TRUE(t2 == t3);

    std::cout << t2.to_string() << std::endl;
}

TEST(test, inverse3)
{
    using tensor = lts::tensor<double, lts::dimensions<4>, lts::dimensions<4>>;

    tensor t1{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 0.0}};
    tensor t2 = inverse(t1, t1.det());

    EXPECT_EQ(0, t1.det());

    std::cout << t2.to_string() << std::endl;
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

