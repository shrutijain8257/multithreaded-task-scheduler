#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <string>

enum class Priority { HIGH = 0, MEDIUM = 1, LOW = 2 };

struct Task {
    Priority priority;
    std::function<void()> func;
    std::string name;

    bool operator>(const Task& other) const {
        return static_cast<int>(priority) > static_cast<int>(other.priority);
    }
};

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) : stop_(false), ready_(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this, i] { workerLoop(i); });
        }
    }

    ~ThreadPool() { stop(); }

    void submit(const std::string& name, Priority p, std::function<void()> fn) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push({p, std::move(fn), name});
            std::cout << "[SUBMIT]  " << name << "\n";
        }
        if (ready_) cv_.notify_one();
    }

    void start() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ready_ = true;
        }
        std::cout << "\n--- all tasks queued, starting workers ---\n\n";
        cv_.notify_all();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& t : workers_)
            if (t.joinable()) t.join();
        workers_.clear();
    }

private:
    void workerLoop(size_t id) {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] {
                    return stop_ || (!queue_.empty() && ready_);
                });

                if (stop_ && queue_.empty()) return;

                task = queue_.top();
                queue_.pop();
            }
            std::cout << "[RUN]     " << task.name
                      << "  (thread " << id << ")\n";
            task.func();
            std::cout << "[DONE]    " << task.name << "\n";
        }
    }

    using PQ = std::priority_queue<Task,
                                   std::vector<Task>,
                                   std::greater<Task>>;
    PQ                       queue_;
    std::mutex               mutex_;
    std::condition_variable  cv_;
    std::atomic<bool>        stop_;
    std::atomic<bool>        ready_;
    std::vector<std::thread> workers_;
};