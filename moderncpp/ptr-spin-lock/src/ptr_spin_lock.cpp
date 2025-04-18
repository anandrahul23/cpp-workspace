#include "../include/spin_lock.h"

int main()
{
    // Shared counter
    int shared_counter = 0;

    // Create a PtrSpinlock to protect the shared counter
    Spinlock lock; // Initialize with the address of the counter

    // Number of threads
    const int num_threads = 4;

    // Lambda function for each thread
    auto increment_counter = [&](int thread_id)
    {
        for (int i = 0; i < 100000; ++i)
        {
            lock.lock(); // Acquire the lock
            // Now we can safely access and modify the shared counter
            ++shared_counter; // Increment the shared counter
            std::cout << "Thread " << thread_id << ": Counter = " << shared_counter << std::endl;
            lock.unlock(); // Release the lock
        }
    };

    // Create and launch threads
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(increment_counter, i);
    }

    // Join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Print the final counter value
    std::cout << "Final counter value: " << shared_counter << std::endl;

    return 0;
}
// This code demonstrates a simple usage of the PtrSpinlock class to protect a shared counter
// from concurrent access by multiple threads. Each thread increments the counter 100,000 times,
// and the final value is printed at the end. The PtrSpinlock ensures that only one thread can