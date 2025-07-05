#pragma once
#include "ExecutorThreadPool.h"
#include <memory>
#include <future>
#include <type_traits>
#include <functional>
#include <exception>

using namespace std;


template <typename T> 
concept SupportsPromise = is_copy_constructible_v<T> ||
                         is_move_constructible_v<T> ||
                         is_reference_v<T> ||
                         is_same_v<T, void>;



template <typename FucntionType, typename ArgType>
concept UnaryFunction = is_invocable_v<FucntionType, ArgType>;


template <SupportsPromise result_t>
class STask : public ExecutableTask
{

public:
[[nodiscard]]
auto get_future()
{
    bool expected{false};
    if(future_accessed.compare_exchange_strong(expected, true, memory_order_acq_rel))
    {
        return task_promise.get_future();
    }
    else
    {
        throw runtime_error("already got a fture from promise, can't do it again");
    }
}
template <typename CallableType, typename... ArgsType>
    requires CallableSupport<CallableType, ArgsType...>
STask(ExeccutableThreadPool& ex, CallableType &&callable, ArgsType &&...args): executor_(ex)
{
    cout << "STask () callsed"<<endl;

    //Too do std::forward doesnt work well with lambda capture for args 
    // as all args are captured by value , not ideal for large object
    //replace with tuples and find out why 
    auto callableLambda = [f = std::forward<CallableType>(callable), ... args = std::forward<ArgsType>(args)]() mutable
    {
        cout << "callableLambda called"<<endl;
        return std::invoke(f, args...);
    };
    fn_ = callableLambda;
}

    void execute() noexcept override
    {
        try
        {
            execute_impl();
        }
        catch(...)
        {
            task_promise.set_exception(std::current_exception());
        }
    }
template <UnaryFunction<result_t> FunctionType>
auto then(FunctionType&& fun)
{
    using ResultT = invoke_result_t<FunctionType, result_t>;
    auto lambdaCallable = [parent = this->shared_from_this(), f = std::forward<FunctionType>(fun)]()mutable
    {
        auto& parentTask = static_cast<STask<result_t>&>(*parent); //safe to do static cast
        //handle void return type
        if constexpr(is_same_v<result_t, void>)
        {
            parentTask.get_future().get();
            f();
        }
        else
        {
            auto res = parentTask.get_future().get();
            return std::invoke(f, std::move(res));  
        }
        
        
    } ;
    auto nextTask = make_shared<STask<ResultT>>(executor_, lambdaCallable);
    executor_.scheduleTask(nextTask);
    return nextTask;
}

    private : 
    void execute_impl()
    {
        cout << "execute_impl called"<<endl;
         if constexpr (is_same_v<result_t, void>)
        {
            cout << "inside execute_impl for void return type"<<endl;
             fn_();
            task_promise.set_value();
        }
        else
        {
            cout << "inside execute_impl for other return type" << endl;
            task_promise.set_value(fn_());
        }
    }

    function<result_t()> fn_;
    promise<result_t> task_promise; 
    atomic<bool> future_accessed{false};

    //ExecutableTaskPtr next_task_; 
    //to schedule the next task
    ExeccutableThreadPool &executor_;
}; 

//helper function to scedule task

template <typename CallableType, typename... ArgsType>
    requires CallableSupport<CallableType, ArgsType...>
auto globalScheduleTask(ExeccutableThreadPool& ex, CallableType &&callable, ArgsType &&...args)
{
    cout<<"scheduling a Task..."<<endl;
    using resultT = invoke_result_t<CallableType, ArgsType...>;
    auto taskPtr = make_shared <STask<resultT>>(ex, std::forward<CallableType>(callable), std::forward<ArgsType>(args)...);

    ex.scheduleTask(taskPtr);

    return taskPtr;
}
