#include "profiler/profiler.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <x86intrin.h>

namespace profiler {

profiler global_profiler = {};
u32 global_profiler_parent_index = 0;

profile_block::profile_block(const char *name, u32 anchor_index) {
    m_name = name;
    m_anchor_index = anchor_index;
    m_parent_index = global_profiler_parent_index;

    profile_anchor *anchor = global_profiler.anchors + anchor_index;
    m_old_tsc_elapsed_inclusive = anchor->tsc_elapsed_inclusive;

    global_profiler_parent_index = m_anchor_index;
    m_start_tsc = __rdtsc();
}

profile_block::~profile_block() {
    u64 elapsed = __rdtsc() - m_start_tsc;
    global_profiler_parent_index = m_parent_index;

    profile_anchor *parent = global_profiler.anchors + m_parent_index;
    profile_anchor *anchor = global_profiler.anchors + m_anchor_index;

    parent->tsc_elapsed_exclusive -= elapsed;
    anchor->tsc_elapsed_exclusive += elapsed;
    anchor->tsc_elapsed_inclusive = m_old_tsc_elapsed_inclusive + elapsed;

    anchor->hits++;
    anchor->name = m_name;
}

uint64_t estimate_cpu_timer_freq() {
    constexpr auto MEASUREMENT_PERIOD = std::chrono::milliseconds(100);
    constexpr uint64_t MICROSECONDS_PER_SECOND = 1'000'000ULL;

    uint64_t cpu_start = __rdtsc();
    auto os_start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - os_start < MEASUREMENT_PERIOD) {
    }

    uint64_t cpu_ticks = __rdtsc() - cpu_start;
    auto os_duration = std::chrono::steady_clock::now() - os_start;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(os_duration).count();

    return MICROSECONDS_PER_SECOND * cpu_ticks / microseconds;
}

void start_profile() { global_profiler.start_tsc = __rdtsc(); }

void end_and_print_profile() {
    global_profiler.end_tsc = __rdtsc();

    u64 cpu_freq = estimate_cpu_timer_freq();
    u64 cpu_elapsed = global_profiler.end_tsc - global_profiler.start_tsc;

    std::vector<size_t> anchor_indices;
    for (size_t i = 0; i < 4096; ++i) {
        if (global_profiler.anchors[i].tsc_elapsed_inclusive) {
            anchor_indices.push_back(i);
        }
    }

    std::sort(anchor_indices.begin(), anchor_indices.end(), [&](size_t a, size_t b) {
        return global_profiler.anchors[a].tsc_elapsed_exclusive >
               global_profiler.anchors[b].tsc_elapsed_exclusive;
    });

    // clang-format off
    std::cout << "\nTotal time: " << 1000.0 * cpu_elapsed / cpu_freq 
              << "ms (CPU freq " << cpu_freq << ")\n\n";

    std::cout << std::left << std::setw(20) << "Function"
              << std::right << std::setw(8) << "Calls"
              << std::setw(12) << "Time(ms)"
              << std::setw(12) << "Self(%)"
              << std::setw(12) << "Total(%)" << '\n'
              << std::string(64, '-') << '\n';
    // clang-format on

    for (size_t idx : anchor_indices) {
        const auto &anchor = global_profiler.anchors[idx];

        double ms = 1000.0 * anchor.tsc_elapsed_exclusive / cpu_freq;
        double self_percent = 100.0 * anchor.tsc_elapsed_exclusive / cpu_elapsed;
        double total_percent = 100.0 * anchor.tsc_elapsed_inclusive / cpu_elapsed;

        // clang-format off
        std::cout << std::left << std::setw(20) << anchor.name
                  << std::right << std::setw(8) << anchor.hits
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << ms
                  << std::setw(11) << self_percent << '%'
                  << std::setw(12) << (anchor.tsc_elapsed_inclusive != anchor.tsc_elapsed_exclusive 
                                     ? std::to_string(total_percent) + "%" : "-")
                  << '\n';
        // clang-format on
    }
}

} // namespace profiler
