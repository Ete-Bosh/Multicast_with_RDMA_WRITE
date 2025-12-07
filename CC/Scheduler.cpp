#include "Scheduler.hpp"

Scheduler::Scheduler() : running_(true), th_(&Scheduler::loop_, this) {}

Scheduler::~Scheduler() {
    stop(); 
}

Scheduler::TaskId Scheduler::add(std::function<void()> fn, us delay) {
    const auto when = Clock::now() + delay;
    std::lock_guard<std::mutex> lk(m_);
    TaskId id = ++next_id_;
    heap_.push(Item{when, id, std::move(fn)});
    cv_.notify_all();
    return id;
}

void Scheduler::cancel(TaskId id) {
    std::lock_guard<std::mutex> lk(m_);
    cancelled_.insert(id); // mark as canceled
    cv_.notify_all();
}

void Scheduler::stop() {
    if (running_.exchange(false)) {
        cv_.notify_all();
        if (th_.joinable()) th_.join();
    }
}

void Scheduler::loop_() {
    std::unique_lock<std::mutex> lk(m_);
    while (running_) {
        if (heap_.empty()) {
            cv_.wait(lk, [&]{ return !running_ || !heap_.empty(); });
            if (!running_) break;
        }

        auto next = heap_.top().when;
        if (cv_.wait_until(lk, next, [&]{ return !running_; })) break;

        if (!running_) break;

        auto now = Clock::now();
        if (heap_.empty() || heap_.top().when > now) continue;

        Item it = heap_.top();
        heap_.pop();
        if (cancelled_.erase(it.id)) continue;

        auto fn = std::move(it.fn);
        lk.unlock();
        fn(); // execute user function
        lk.lock();
    }
}
