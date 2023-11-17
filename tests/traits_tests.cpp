
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