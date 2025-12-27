#pragma once

#include <cstdint>
#include <string>
#include <concepts>
#include <expected>
#include <limits>

enum class parse_error {
    invalid_character,
    overflow
};

std::uint32_t parse_uint32_swar(const std::string& str);

template<std::unsigned_integral U>
std::expected<U, parse_error> parse_uint_predictable(const char* ptr) { 
    constexpr U MAX = std::numeric_limits<U>::max();
    constexpr U MAX_DIV_10 = MAX / 10;
    constexpr U MAX_MOD_10 = MAX % 10;

    U result = 0;
    while (*ptr) [[likely]] {
        char c = *ptr++;
        if (c < '0' || c > '9') return std::unexpected(parse_error::invalid_character);

        U i = (c - '0');

        if (result < MAX_DIV_10 || (result == MAX_DIV_10 && i <= MAX_MOD_10)) {
            result = result * 10 + i;
        } else {
            return std::unexpected(parse_error::overflow);
        }
    }

    return result;
}

