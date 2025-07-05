#pragma once
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <utility>
#include <exception>


using namespace std;

//class can accept any callable tasks 

template <typename CallableType , typename... ArgsType>
concept SupportsInvocable = is_invocable_v<CallableType, ArgsType...>; 

class ThreadPool
{
    public: 
    using Task = function<void()>;
    template <typename CallableType, typename... ArgsType>
        requires SupportsInvocable<CallableType, ArgsType...>
    auto scheduleTask(CallableType&& callable, ArgsType&&... args); // lets implement it outside class to get familiar with the syntax

    void run_thread(); 

    ThreadPool(size_t n = 4)
    {
        size_t tempNo = std::max(4U, thread::hardware_concurrency());
        n = std::min(n , tempNo);
        for(int i =0; i < n; ++i)
        {
            threads_.emplace_back(&ThreadPool::run_thread, this);
        }
    }

    ~ThreadPool(); //define it outside for practice 


    private:

    //
    vector<thread> threads_; 
    queue<Task> tasks_; 
    mutex qMutex_; 
    // just a consumer here , producer will be outside this class and will be 
    //able to schedule the task 
    condition_variable consumer_ready; 

    atomic<bool> shut_down{false}; 


};


ThreadPool::~ThreadPool()
{
    shut_down.store(true, memory_order_release); //for graceful shutdown

    consumer_ready.notify_all(); //notify all threads
    for(auto& t: threads_)
    {
        if(t.joinable())
        {
            t.join();
        }
    }//wait for all threads to finish work; 
}


void ThreadPool::run_thread()
{
    while(true)
    {
        unique_lock<mutex> qLock{qMutex_};

        consumer_ready.wait(qLock, [this]() 
                                    { return (!tasks_.empty() || shut_down.load(memory_order_acquire));
                                    }); 
        if(tasks_.empty() && shut_down.load(memory_order_acquire))
        {
            break;
        }
        auto task = std::move(tasks_.front());
        tasks_.pop();
        qLock.unlock(); 
        task();
    }
}

template <typename CallableType, typename... ArgsType>
    requires SupportsInvocable<CallableType, ArgsType...>
auto ThreadPool::scheduleTask(CallableType&& callable, ArgsType&&... args)
{

    if(shut_down.load(memory_order_acquire))
    {
        throw std::runtime_error(" can not schedule any more task when Thread pool is shutting down");
    }
    using Result_t = invoke_result_t<CallableType, ArgsType...>;

    //alternatively 
    //using Result_t = decltype(callable(args..));

    auto lambda_invocable = [f = std::forward<CallableType>(callable), ...lambda_args = std::forward<ArgsType>(args)]() mutable
    {
        return invoke(f, lambda_args...); 
        // return f(lambda_args...);
    };

    auto pkgdTaskPtr = make_shared<packaged_task<Result_t()>>(lambda_invocable); 

    //get the future of the task
    auto task_future = pkgdTaskPtr->get_future();

    // now enqueu to the queue 
    {
        scoped_lock<mutex> queue_lock{qMutex_};
        //task is shared_ptr , so fine to pass by copy , it will increase the refrence count
        tasks_.emplace([pkgdTaskPtr]()
                       { (*pkgdTaskPtr)(); });
    }
    consumer_ready.notify_one();

    return task_future;    
}