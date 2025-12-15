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

void benchmark_throughput(std::size_t iterations)  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> lower_distribution(0U, 9'999U);
    std::uniform_int_distribution<uint32_t> middle_distribution(10'000U, 9'999'999U);
    std::uniform_int_distribution<uint32_t> upper_distribution(10'000'000U, 4'294'967'294U);

    auto make_range = [&](size_t n, auto& dist) {
        return std::views::iota(0u, static_cast<uint32_t>(n))
             | std::views::transform([&](auto) { return dist(gen); })
             | std::views::transform([](auto i) { return std::to_string(i); });;
    };

    auto variable_size_sets = {
        std::pair{"LOWER   (4 char) ", make_range(iterations, lower_distribution)},
        std::pair{"MIDDLE  (7 char) ", make_range(iterations, middle_distribution)},
        std::pair{"UPPER (> 8 char) ", make_range(iterations, upper_distribution)}
    };

    std::println("===== BENCHMARK THROUGHPUTS =====");

    volatile std::uint32_t sink;

    for (const auto& [name, set]: variable_size_sets) {
        std::println("{}", name);
        double tottime;
        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& str: set) 
                sink = std::stol(str);
        }
        std::println("std::stoi    :: {:7.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));

        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& str: set) 
                sink = parse_uint_predictable<std::uint32_t>(str.c_str());
        }
        std::println("Predicatable :: {:7.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));

        {
            auto _ = ScopeTimer(&tottime);
            for (const auto& str: set) 
                sink = parse_uint32_swar(str);
        }
        std::println("SWAR         :: {:7.0f} ints/ms", calculate_throughput_per_ms(tottime, set.size()));
    }
}

int main() {
    validate_correctness();
    benchmark_throughput(100'000'000);

    return 0;
}
