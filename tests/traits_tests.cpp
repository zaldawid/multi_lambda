
#include <type_traits>
#include <gtest/gtest.h>
#include "multi_lambda.h"

TEST(head_t, single_element)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::head_t<int>, int>)) << "head_t<int> should be int";
    EXPECT_FALSE((std::is_same_v<functors::traits::head_t<int>, double>)) << "head_t<int> should not be double";
}

TEST(head_t, multi_elements)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::head_t<int, double>, int>)) << "head_t<int, double> should be int";
    EXPECT_FALSE((std::is_same_v<functors::traits::head_t<int, double>, double>)) << "head_t<int, double> should not be double";
}

TEST(first_v, single_element)
{
    EXPECT_EQ(functors::traits::first_v<0>, 0) << "first_v<0> should be 0";
    EXPECT_NE(functors::traits::first_v<0>, 1) << "first_v<0> should not be 1";
}

TEST(first_v, multi_elements)
{
    EXPECT_EQ((functors::traits::first_v<0, 1, 2>), 0) << "first_v<0, 1, 2> should be 0";
    EXPECT_NE((functors::traits::first_v<0, 1, 2>), 1) << "first_v<0, 1, 2> should not be 1";
}

TEST(nth_type_t, single_element)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::nth_type_t<0, int>, int>)) << "nth_type_t<0, int> should be int";
    EXPECT_FALSE((std::is_same_v<functors::traits::nth_type_t<0, int>, double>)) << "nth_type_t<0, int> should not be double";
}

TEST(nth_type_t, multi_elements)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::nth_type_t<1, int, double, unsigned>, double>)) << "nth_type_t<1, int, double, unsigned> should be double";
    EXPECT_FALSE((std::is_same_v<functors::traits::nth_type_t<1, int, double, unsigned>, int>)) << "nth_type_t<1, int, double, unsigned> should not be int";
}

template <typename...>
struct typelist;

TEST(nth_type_t, single_element_typelist)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::nth_type_t<0, typelist<int>>, int>)) << "nth_type_t<0, typelist<int>> should be int";
    EXPECT_FALSE((std::is_same_v<functors::traits::nth_type_t<0, typelist<int>>, double>)) << "nth_type_t<0, typelist<int>> should not be double";
}

TEST(nth_type_t, multi_elements_typelist)
{
    EXPECT_TRUE((std::is_same_v<functors::traits::nth_type_t<1, typelist<int, double, unsigned>>, double>)) << "nth_type_t<1, typelist<int, double, unsigned>> should be double";
    EXPECT_FALSE((std::is_same_v<functors::traits::nth_type_t<1, typelist<int, double, unsigned>>, int>)) << "nth_type_t<1, typelist<int, double, unsigned>> should not be int";
}