#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>
using namespace  std;

template <typename T>
class ThreadSafeQueue
{

    public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue& rhs) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete; 
    ~ThreadSafeQueue()
    {
        shutDown();
    }
    void push(const T& msg)
    {
            if(shut_down.load(memory_order_acquire))
            {
                //throw exception; 
                return; 
            }
        {
            scoped_lock<mutex> lk{qMutex_};
            messages_.emplace(msg);
        }
       
        consumer_ready.notify_one();
    }

    optional<T> try_and_pop()
    {
        scoped_lock<mutex> lk{qMutex_};
        if(messages_.empty())
        {
            return nullopt;
        }
        auto msg = std::move(messages_.front());
        messages_.pop();
        return msg;
    }

    optional<T> wait_and_pop()
    {
        unique_lock<mutex> lk{qMutex_};
        consumer_ready.wait(lk, [this]() 
                                { return (!messages_.empty() || shut_down.load(memory_order_acquire));
                        });
        if(messages_.empty && shut_down.load(memory_order_acquire))
        {
            return nullopt;
        }
        auto msg = std::move(messages_.front());
        messages_.pop();
        return msg;
    }

    void shutDown()
    {
        shut_down.store(true, memory_order_release);
        consumer_ready.notify_all();
    }

    bool isEmpty() const
    {
        scoped_lock<mutex> lk{qMutex_};
        return messages_.empty();
    }



    private:

    mutable mutex qMutex_; 
    queue<T> messages_; 
    condition_variable consumer_ready; 
    atomic<bool> shut_down{false};

};
