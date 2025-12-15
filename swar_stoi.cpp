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

// template<std::uint8_t N>
// std::uint32_t parse_uint_simd_comp(const char (&ptr)[N]) { 
//     static_assert(1 <= N && N <= 8);
// 
//     // std::uint64_t chunk = 0;
//     // std::memcpy(&chunk, ptr, len);
//     constexpr std::uint64_t mask = (1ULL << (N << 3)) - 1;
//     constexpr std::uint64_t zero_mask = 0x3030303030303030ULL & mask;
// 
//     std::uint64_t chunk = (
//         (*reinterpret_cast<const std::uint64_t*>(ptr) & mask) 
//         - zero_mask
//     );
// 
//     chunk = (chunk * 10    + (chunk >> 8))  & 0x00FF00FF00FF00FFULL;
//     chunk = (chunk * 100   + (chunk >> 16)) & 0x0000FFFF0000FFFFULL;
//     chunk = (chunk * 10000 + (chunk >> 32)) & 0x00000000FFFFFFFFULL;
// 
//     static constexpr std::uint32_t pow10[9] = {
//         1u,
//         10u,
//         100u,
//         1000u,
//         10000u,
//         100000u,
//         1000000u,
//         10000000u,
//         100000000u
//     };
//     constexpr std::uint32_t scale = pow10[8 - N];
// 
//     return static_cast<std::uint32_t>(chunk / scale);
// }
