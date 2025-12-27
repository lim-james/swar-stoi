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

std::uint32_t parse_uint32_swar(const std::string& str) { 
    auto ptr = str.data();

    if (str.length() < 8) [[likely]] {
        std::uint32_t result = 0;
        while (*ptr) [[likely]] result = result * 10 + (*ptr++ - '0');
        return result;
    }

    std::uint64_t chunk = *reinterpret_cast<const std::uint64_t*>(ptr) - 0x3030303030303030ULL;
    chunk = (chunk * 10    + (chunk >> 8))  & 0x00FF00FF00FF00FFULL;
    chunk = (chunk * 100   + (chunk >> 16)) & 0x0000FFFF0000FFFFULL;
    chunk = (chunk * 10000 + (chunk >> 32)) & 0x00000000FFFFFFFFULL;
    ptr += 8;

    while (*ptr) chunk = chunk * 10 + (*ptr++ - '0');

    return chunk;
}

template<std::unsigned_integral U>
std::expected<U, parse_error> parse_uint_predictable(const std::string& str) { 
    constexpr U MAX = std::numeric_limits<U>::max();
    constexpr U MAX_DIV_10 = MAX / 10;
    constexpr U MAX_MOD_10 = MAX % 10;

    U result = 0;
    auto ptr = str.data();
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

