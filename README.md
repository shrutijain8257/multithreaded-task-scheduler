# Multithreaded Priority Task Scheduler

A C++17 thread pool with priority-based task scheduling.

## Features
- 3 priority levels: HIGH, MEDIUM, LOW (min-heap via std::priority_queue)
- Zero busy-waiting using std::condition_variable
- Separate submit and start phases to guarantee priority ordering
- Clean shutdown via atomic stop flag + notify_all

## Build & Run
g++ -std=c++17 -pthread -o scheduler main.cpp && ./scheduler

## Key Concepts
- std::mutex guards the shared priority queue
- std::unique_lock + condition_variable.wait() for efficient thread sleeping
- atomic<bool> for stop_ and ready_ flags (lock-free reads)
- start() pattern separates batch submission from worker execution
