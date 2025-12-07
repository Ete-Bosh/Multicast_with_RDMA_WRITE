#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>

class Scheduler {
public:
    using us = std::chrono::microseconds;
    using Clock = std::chrono::steady_clock;
    using TaskId = std::uint64_t;

    Scheduler();
    ~Scheduler();

    TaskId add(std::function<void()> fn, us delay);
    void cancel(TaskId id);
    void stop();

private:
    struct Item {
        Clock::time_point when;
        TaskId id;
        std::function<void()> fn;
        bool operator<(const Item& o) const { return when > o.when; }
    };

    void loop_();

    std::atomic<bool> running_{false};
    std::thread th_;
    std::mutex m_;
    std::condition_variable cv_;
    std::priority_queue<Item> heap_;
    std::unordered_set<TaskId> cancelled_;
    TaskId next_id_{0};
};
