#pragma once

#include <cstdint>
#include <string>

std::uint32_t parse_uint_simd(const std::string& str);
std::uint32_t parse_uint_branch_friendly(const char* ptr);

