#include "swar_stoi.h"

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

