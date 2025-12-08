#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <cstddef>

using namespace std;
template <typename std::size_t N , typename std::size_t MAX = 100> 
class ThreadSequence
{
public:
ThreadSequence()
{
        size_t threadIndex = 0U; 
        //the first lambda is  to generate the threads in array 
        //inside the thread constructor , lambda is to spawn the thread 
        // std::generate(threads_.begin(), threads_.end(), [this, &threadIndex]() mutable {

        //     return jthread{[this, i{threadIndex++}]()
        //         {
        //         printInSequence(i);
        //      };  //thread lambda end 
        // }; // jthread constructor end 
        // });

        std::ranges::generate(threads_, [this, &threadIndex]() mutable
                      {
                          return jthread
                          (
                              [this, i{threadIndex++}]() mutable
                              {
                                  this->printInSequence(i);
                              } // thread lambda end
                            ); // jthread constructor end
                      } //outer lambda for generate 
                      );
}



private:

    void notifyAllThreads()
    {
        for(auto& cv: con_variables_)
        {
            cv.notify_one();
        }
    }
    void notifyAllThreadsExceptCurrent(size_t threadNo)
    {
        // notify all threads , except the current one
        for (size_t i = 0U; i < N; ++i)
        {
            if (i != threadNo)
                con_variables_[i].notify_one();
        }
    }

void printInSequence(size_t threadNo)
{
    while (true)
    {
        unique_lock<mutex> lk{mutex_};

        con_variables_[threadNo].wait(lk, [this, threadNo]()
                                      { 
                                        size_t conut =  counter_.load(memory_order_acquire);
                                        return (threadNo == conut % N || conut >= MAX); 
                                    });

        if (counter_.load(memory_order_acquire) >= MAX)
        {
            // this is a signal to stop all threads
            // notify all threads waiting on its own condition variable to wake up and exit the current thread
            //notifyAllThreads();
            notifyAllThreadsExceptCurrent(N);
            return;
        }
        counter_.fetch_add(1, memory_order_release);
        cout << "Thread:" << threadNo << " counter:" << counter_ << endl;
       

        lk.unlock();

        //notify next thread only
        size_t nextThId = (threadNo+1) % N;
        con_variables_[nextThId].notify_one();
    }
}

array<condition_variable, N> con_variables_{}; 
mutex mutex_; 
//jthread will automatically join on its destruction 
array<jthread, N> threads_; 
atomic<size_t> counter_{0U}; 

};
