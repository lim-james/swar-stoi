#include <gtest/gtest.h>

#include <cstdint>

#include "swar_stoi.hpp"

class StoiFuzz : public ::testing::TestWithParam<std::string> {};

TEST_P(StoiFuzz, InvalidValue) {
    auto result = stou<std::uint32_t>(GetParam());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), parse_error::invalid_character);
}

INSTANTIATE_TEST_SUITE_P(
    StoiFuzzTest,
    StoiFuzz,
    ::testing::Values("abc", "123a", "-5000")
);

