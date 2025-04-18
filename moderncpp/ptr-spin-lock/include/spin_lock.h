#pragma once
#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

class Spinlock
{
public:
    explicit Spinlock() : flag_{ATOMIC_FLAG_INIT}
    {

    } // Constructor initializes the atomic flag

    void lock()
    {
        while (flag_.test_and_set(std::memory_order_acquire))
        {
            // Spin (busy-wait) until the lock is acquired
            std::this_thread::yield(); // Optional: Yield to reduce CPU consumption
        }
    }

    void unlock()
    {
        flag_.clear(std::memory_order_release); // Release the lock
    }
    // Deleted copy constructor and assignment operator to prevent copying
    Spinlock(const Spinlock &) = delete;
    Spinlock &operator=(const Spinlock &) = delete;
    // Deleted move constructor and assignment operator to prevent moving
    Spinlock(Spinlock &&) = delete;
    Spinlock &operator=(Spinlock &&) = delete;

private:
    std::atomic_flag flag_; // Atomic flag for lock state
};
