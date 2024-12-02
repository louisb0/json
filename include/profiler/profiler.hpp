#pragma once

#include <cstdint>
#include <cstdio>
#include <sys/time.h>

#ifndef PROFILER
#define PROFILER 0
#endif

using u32 = uint32_t;
using u64 = uint64_t;

namespace profiler {

struct profiler {
    u64 start_tsc;
    u64 end_tsc;
};

extern profiler global_profiler;

uint64_t estimate_cpu_timer_freq();
void start_profile();
void end_and_print_profile();

#if PROFILER

struct profile_anchor {
    const char *name;
    u64 hits;
    u64 tsc_elapsed_exclusive;
    u64 tsc_elapsed_inclusive;
};

extern u32 global_profiler_parent_index;
extern profile_anchor anchors[4096];

class profile_block {
public:
    profile_block(const char *name, u32 anchor_index);
    ~profile_block();

    profile_block(const profile_block &) = delete;
    profile_block &operator=(const profile_block &) = delete;
    profile_block(profile_block &&) = delete;
    profile_block &operator=(profile_block &&) = delete;

private:
    const char *m_name;
    u32 m_anchor_index;
    u32 m_parent_index;
    u64 m_old_tsc_elapsed_inclusive;
    u64 m_start_tsc;
};

constexpr u32 hash(const char *str) {
    u32 hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + *str++;
    }
    return (hash % 4095) + 1;
}

#define CONCAT2(A, B) A##B
#define CONCAT(A, B) CONCAT2(A, B)
#define PROFILE_BLOCK(name)                                                                         \
    profiler::profile_block CONCAT(block, __LINE__)(name, profiler::hash(name))
#define PROFILE_FUNCTION() PROFILE_BLOCK(__func__)

#else

#define PROFILE_BLOCK(name)
#define PROFILE_FUNCTION()

#endif // PROFILER

} // namespace profiler
