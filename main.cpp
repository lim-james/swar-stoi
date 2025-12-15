#include <array>
#include <csignal>
#include <print>
#include <chrono>
#include <cassert>
#include <ranges>
#include <algorithm>
#include <utility>
#include <random>
#include <format>
#include <limits>

#include "swar_stoi.h"

class [[nodiscard]] ScopeTimer {
private:
    double* out_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;

public:
    ScopeTimer(double* out)
        : out_(out)
        , start_(std::chrono::high_resolution_clock::now()) {}

    ~ScopeTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        *out_ = std::chrono::duration<double, std::milli>(end - start_).count();
    }
};

constexpr double calculate_throughput_per_s(double tottime_ms, std::size_t ncalls) {
    constexpr double ms_to_s = 1'000.0;
    return static_cast<double>(ncalls) / tottime_ms * ms_to_s; 

}

constexpr double calculate_throughput_per_ms(double tottime_ms, std::size_t ncalls) {
    return static_cast<double>(ncalls) / tottime_ms; 

}

void validate_correctness() {
    for (std::uint32_t i{}; i < std::numeric_limits<std::uint32_t>::max(); ++i) 
        assert(parse_uint32_swar(std::to_string(i)) == i);
    std::println("Validated swar implementation across 0...{}", std::numeric_limits<std::uint32_t>::max());

    for (std::uint32_t i{}; i < std::numeric_limits<std::uint32_t>::max(); ++i) 
        assert(parse_uint_predictable<std::uint32_t>(std::to_string(i).c_str()) == i);
    std::println("Validated small implementation across 0...{}", std::numeric_limits<std::uint32_t>::max());
}

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> lower_distribution(0U, 9'999U);
    std::uniform_int_distribution<uint32_t> middle_distribution(10'000U, 9'999'999U);
    std::uniform_int_distribution<uint32_t> fixed_len_distribution(1'000U, 9'999U);
    std::uniform_int_distribution<uint32_t> upper_distribution(10'000'000U, 4'294'967'294U);

    auto make_str_int_pair = std::views::transform([](auto i) {
        return std::make_pair(std::to_string(i), i);
    });

    auto make_range = [&](size_t n, auto& dist) {
        return std::views::iota(0u, static_cast<uint32_t>(n))
             | std::views::transform([&](auto) { return dist(gen); })
             | make_str_int_pair;
    };

    auto variable_size_sets = {
        // std::pair{"FIXED  ",  make_range(ITERATIONS, fixed_len_distribution)},
        std::pair{"LOWER  ", make_range(ITERATIONS, lower_distribution)},
        std::pair{"MIDDLE ", make_range(ITERATIONS, middle_distribution)},
        std::pair{"UPPER  ", make_range(ITERATIONS, upper_distribution)}
    };

    std::println("===== THROUGHPUTS =====");

    for (const auto& [name, set]: variable_size_sets) {
        double tottime;
        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& [str, i]: set) 
                assert(i == std::stol(str));
        }
        std::println("std::stoi    :: {:.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));

        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& [str, i]: set) 
                assert(i == parse_uint_predictable<std::uint32_t>(str.c_str()));
        }
        std::println("Predicatable :: {:.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));

        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& [str, i]: set) 
                assert(i == parse_uint_swar(str));
        }
        std::println("SWAR         :: {:.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));

        std::println();
    }

    return 0;
}
