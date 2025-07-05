#pragma once
#include <memory>
#include <thread>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <iostream>

using namespace std;

template <typename CallableType, typename... ArgsType>
concept CallableSupport = is_invocable_v<CallableType, ArgsType...>;

class ExecutableTask : public std::enable_shared_from_this<ExecutableTask>
{

public:
    virtual ~ExecutableTask() = default;

    virtual void execute() noexcept = 0;
};

using ExecutableTaskPtr = shared_ptr<ExecutableTask>;

//not makin
class ExeccutableThreadPool
{

public:


    ExeccutableThreadPool(size_t  noOfThreads)
    {
        if(noOfThreads < 2)
        {
            noOfThreads = 2;
        }
        size_t n = thread::hardware_concurrency();
        n = min(noOfThreads, n);
        //schedule the threads; 
        for (int i =0; i < n; ++i)
        {
            threadPool_.emplace_back(&ExeccutableThreadPool::run_task, this);
        }
    }
    ~ExeccutableThreadPool()
    {
        shut_down.store(true, memory_order_release);
        consumer_ready.notify_all(); 
        for(auto& th: threadPool_)
        {
            th.join();
        }

    }
    
    void scheduleTask(ExecutableTaskPtr executablePtr)
    {   
        if(shut_down.load(memory_order_acquire))
        {
            throw runtime_error("cant schedule job when , thread Pool executor is shutting down");
        }
        
        {
            scoped_lock<mutex> qLk{qMutex_};
            taskQueue.push(std::move(executablePtr));
        }

        consumer_ready.notify_one();
    }

    void
    run_task()
    {
        while (true)
        {
        unique_lock<mutex> uLk { qMutex_ };
        consumer_ready.wait(uLk, [this]()
                        { return (!taskQueue.empty() || shut_down.load(memory_order_acquire)); });

        if (taskQueue.empty() && shut_down.load(memory_order_acquire))
        {
            return; // come out of thread
        }
            auto task = std::move(taskQueue.front());
            taskQueue.pop();
            std::cout<<"executing task in thread id:"<<this_thread::get_id()<<endl;

            uLk.unlock(); // release mutex before executing task

            task->execute();
        }
       
    }

private:
    atomic<bool> shut_down{false};
    mutex qMutex_;
    condition_variable consumer_ready;
    queue<ExecutableTaskPtr> taskQueue;
    vector<thread> threadPool_;
}; 