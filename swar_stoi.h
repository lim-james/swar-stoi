#pragma once

#include <cstdint>
#include <string>

constexpr std::uint64_t char_mask(std::uint8_t characters);

std::uint32_t parse_uint_simd(const std::string& str);
std::uint32_t parse_uint_branch_friendly(const char* ptr);

