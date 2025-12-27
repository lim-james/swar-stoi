#include <concepts>
#include <cstdint>
#include <cassert>
#include <limits>
#include <string>
#include <print>

#include "swar_stoi.hpp"

template<std::unsigned_integral T>
void validate_correctness(auto&& stoi_method) {
    constexpr T MAX = std::numeric_limits<std::uint32_t>::max();
    for (T i{}; i < MAX; ++i) 
        assert(stoi_method(std::to_string(i)) == i);
}

int main() {
    validate_correctness<std::uint32_t>(&parse_uint_predictable<std::uint32_t>);
}
