#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <string>

#include "swar_stoi.hpp"

///
/// Acceptable values
///

template<typename T>
class StoiValidTest : public ::testing::TestWithParam<T> {};

#define REGISTER_STOI_VALID_TEST(TypeName, T, ...)                       \
    using StoiValidTest##TypeName = StoiValidTest<T>;                    \
                                                                         \
    TEST_P(StoiValidTest##TypeName, CorrectValue) {                      \
        T expected = GetParam();                                         \
        std::string input  = std::to_string(expected);                   \
        auto result = stou<T>(input);                                    \
        ASSERT_TRUE(result.has_value()) << "Failed to parse: " << input; \
        EXPECT_EQ(*result, expected) << "Input was: " << input;          \
    }                                                                    \
    INSTANTIATE_TEST_SUITE_P(                                            \
        StoiValid##TypeName,                                             \
        StoiValidTest##TypeName,                                         \
        ::testing::Values(__VA_ARGS__)                                   \
    );

REGISTER_STOI_VALID_TEST(Uint32, std::uint32_t, 0, 1, 67, 123456789)
REGISTER_STOI_VALID_TEST(Uint64, std::uint64_t, 0, 1, 123456789, 17446744073709551615ULL)



///
/// Values on the edge (numerical limit max)
///

template<typename T> 
class StoiEdgeTest : public ::testing::Test {};

using StoiTypes = ::testing::Types<std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(StoiEdgeTest, StoiTypes);

TYPED_TEST(StoiEdgeTest, CorrectValue) {
    TypeParam max_value = std::numeric_limits<TypeParam>::max();
    auto result = stou<TypeParam>(std::to_string(max_value));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, max_value);
}



///
/// Overflow
///

template<typename T>
class StoiOverflowTest : public ::testing::TestWithParam<std::string> {}; 

#define REGISTER_STOI_OVERFLOW_TEST(TypeName, T, ...)       \
    using StoiOverflowTest##TypeName = StoiOverflowTest<T>; \
                                                            \
    TEST_P(StoiOverflowTest##TypeName, OverflowValue) {     \
        auto result = stou<T>(GetParam());                  \
        ASSERT_FALSE(result.has_value());                   \
        EXPECT_EQ(result.error(), parse_error::overflow);   \
    }                                                       \
                                                            \
    INSTANTIATE_TEST_SUITE_P(                               \
        StoiOverflow##TypeName,                             \
        StoiOverflowTest##TypeName,                         \
        ::testing::Values(__VA_ARGS__)                      \
    );

REGISTER_STOI_OVERFLOW_TEST(Uint32, std::uint32_t, 
                            "4294967296", "42949672950", "9999999999")
REGISTER_STOI_OVERFLOW_TEST(Uint64, std::uint64_t, 
                            "18446744073709551616",  "184467440737095516150", "99999999999999999999")
