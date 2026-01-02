#include <gtest/gtest.h>

#include <concepts>
#include <cstdint>
#include <cassert>
#include <limits>
#include <string>
#include <print>

#include "swar_stoi.hpp"

template<std::unsigned_integral T>
void validate_correctness(auto&& stoi_method, T stride = 1) {
    constexpr T MAX = std::numeric_limits<T>::max();
    for (T i{}; i < MAX; i += stride) 
        EXPECT_EQ(stoi_method(std::to_string(i)), i);
}


TEST(StoiCorrectness, DifferentialTestWithStd) {
    // validate_correctness<std::uint32_t>(&stou<std::uint32_t>, 100);
}
