#pragma once

#include <cstdint>
#include <string>
#include <concepts>

std::uint32_t parse_uint32_swar(const std::string& str);

template<std::unsigned_integral U>
U parse_uint_predictable(const char* ptr) { 
    U result = 0;
    while (*ptr) [[likely]] result = result * 10 + (*ptr++ - '0');
    return result;
}

