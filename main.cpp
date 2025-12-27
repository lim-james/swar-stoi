#include <print>
#include <cassert>
#include <ranges>
#include <random>
#include <limits>

#include "scoped_timer.hpp"
#include "perf_event.hpp"

#include "swar_stoi.h"

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
                sink = parse_uint_predictable<std::uint32_t>(str.c_str()).value();
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


void perf_cpu_cycles(std::size_t iterations)  {
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

    std::println("===== CPU CYCLES =====");

    volatile std::uint32_t sink;

    auto record = [](long long* instructions, long long* cpu_cycles) {
        auto perf_instr = PerfEvent::make_event(
            PERF_TYPE_HARDWARE,
            PERF_COUNT_HW_INSTRUCTIONS,
            instructions
        );

        auto perf_cycles = PerfEvent::make_event(
            PERF_TYPE_HARDWARE,
            PERF_COUNT_HW_CPU_CYCLES,
            cpu_cycles
        );

        return std::pair{std::move(perf_instr), std::move(perf_cycles)};
    };

    std::println("Method       | {:10} | {:10}", "Cycles", "Instr");
    for (const auto& [name, set]: variable_size_sets) {
        std::println("{}", name);
        long long instructions{};
        long long cpu_cycles{};
        if (auto [instr,cyc] = record(&instructions, &cpu_cycles); instr && cyc) {
            for (const auto& str: set) 
                sink = std::stol(str);
        }
        std::println("std::stoi    | {:10} | {:10} ", cpu_cycles, instructions);

        if (auto [instr,cyc] = record(&instructions, &cpu_cycles); instr && cyc) {
            for (const auto& str: set) 
                sink = parse_uint_predictable<std::uint32_t>(str.c_str()).value();
        }
        std::println("Predicatable | {:10} | {:10} ", cpu_cycles, instructions);

        if (auto [instr,cyc] = record(&instructions, &cpu_cycles); instr && cyc) {
            for (const auto& str: set) 
                sink = parse_uint32_swar(str);
        }
        std::println("SWAR STOI    | {:10} | {:10} ", cpu_cycles, instructions);
    }
}

int main() {
    validate_correctness();
    benchmark_throughput(100'000);
    perf_cpu_cycles(100'000);

    return 0;
}
