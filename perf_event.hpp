#pragma once

#include <optional>
#include <cstdint>
#include <cstring>
#include <utility>
#include <print>

#include <emmintrin.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <asm/unistd.h>

class [[nodiscard]] PerfEvent {
private:
    static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                                int cpu, int group_fd, unsigned long flags) {
        return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    }

    int fd_ = -1;
    long long* result_;
    PerfEvent(int fd, long long* result): fd_(fd), result_(result) {
        ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
        ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
    }

public:

    PerfEvent(const PerfEvent&) = delete;
    PerfEvent& operator=(const PerfEvent&) = delete;

    PerfEvent(PerfEvent&& other): fd_(other.fd_), result_(other.result_) {
        other.fd_ = -1;
        other.result_ = nullptr;
    }

    PerfEvent& operator=(PerfEvent&& other) {
        if (this == &other)
            return *this;

        fd_ = std::exchange(other.fd_, -1);
        result_ = std::exchange(other.result_, nullptr);

        return *this;
    }

    ~PerfEvent() noexcept {
        if (fd_ != -1) {
            ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
            read(fd_, result_, sizeof(long long));
            close(fd_);
        }
    }

    static std::optional<PerfEvent> make_event(
        std::uint32_t type, 
        std::uint64_t config,
        long long* result
    ) {
        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type = type;
        pe.size = sizeof(pe);
        pe.config = config; 
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        int fd = perf_event_open(&pe, 0, -1, -1, 0);
        if (fd == -1) {
            std::println("Error opening perf event");
            return std::nullopt;
        }

        return PerfEvent(fd, result);
    }
};

