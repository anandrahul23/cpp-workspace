#pragma once
#include "ExecutorThreadPool.h"
#include <memory>
#include <future>
#include <type_traits>
#include <functional>
#include <exception>
#include <utility>

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
    template<UnaryFunction<result_t> FunctionType>
    auto then(FunctionType &&fun)
    {
        using ResultT = invoke_result_t<FunctionType, result_t>;
        auto lambdaCallable = [parent = this->shared_from_this(), f = std::forward<FunctionType>(fun)]() mutable
        {
            auto &parentTask = static_cast<STask<result_t> &>(*parent); // safe to do static cast
            // handle void return type
            if constexpr (is_same_v<result_t, void>)
            {
                parentTask.get_future().get();
                f();
            }
            else
            {
                auto res = parentTask.get_future().get();
                return std::invoke(f, std::move(res));
            }
        };
        auto nextTask = make_shared<STask<ResultT>>(executor_, lambdaCallable);
        executor_.scheduleTask(nextTask);
        return nextTask;
    }

template <UnaryFunction<result_t>... FunTypes>
auto then_fork(FunTypes&&... funcs) //get a no of funs 
{
    //convert funcs to tuple of tasks
    auto tuple_of_tasks = makeTasksTuple(this->task_promise.get_future().share(), std::forward<FunTypes>(funcs)...);
    //now create one final task which will schedule all tasks in tuple and wait for result
    //this will be blocking call to wait for results 
    //for this i need the return type  of this task which 
    //should be a tuple of return values of individual tasks 
    //lets do this in with tuple unpacking
    using fork_join_ret_type = decltype(waitForTasks(tuple_of_tasks));

    auto fork_join_callable = [taskTuples = std::move(tuple_of_tasks), parent{shared_from_this()}, ex{&this->executor_}]() mutable
    {
        parent->schedule_task_tuple(*ex, taskTuples);
        return parent->waitForTasks(taskTuples);
    };

    auto fork_join_task = make_shared<STask<fork_join_ret_type>>(executor_, fork_join_callable);
    executor_.scheduleTask(fork_join_task); 

    return fork_join_task;

}


template <typename... ArgsType>
void schedule_task_tuple(ExeccutableThreadPool &ex, tuple<ArgsType...> &&all_tasks) // this form , because i need Args...
{
    //use std::apply to unpack tuple parameters and apply to some function
     std::apply([this, &ex](auto&&... funcs) 
            {  schedule_task_tuple(ex, std::forward<decltype(funcs)>(funcs)...); }, 
            all_tasks); 
}
template <typename TaskType, typename... TaskTypes>
void schedule_task_tuple(ExeccutableThreadPool &ex, TaskType &&aTask, TaskTypes &&...moreTasks)
{
    schedule_task_tuple(ex, std::forward<TaskType>(aTask));
    schedule_task_tuple(ex, std::forward<TaskTypes>(moreTasks)...);
}

template <typename TaskType>
void schedule_task_tuple(ExeccutableThreadPool &ex, TaskType &&aTask)
{
    ex.scheduleTask(std::move(aTask)); 
}

    // i will move this in to some other utility namespace , but for now
    // implement it here
    template <typename... ArgsType>
    auto waitForTasks(tuple<ArgsType...> &&all_tasks) // this form , because i need Args...
{
        //use std::apply to unpack tuple parameters and apply to some function
        return std::apply([this](auto&&...funcs) 
                { return waitForTasks(funcs...);}, 
                all_tasks); 
}
template <typename TaskType, typename... TaskTypes>
auto waitForTasks(TaskType&& aTask, TaskTypes&&... moreTasks)
{
    return std::tuple_cat(waitForTasks(std::forward<TaskType>(aTask)), waitForTasks(std::forward<TaskTypes>(moreTasks)...));
}

template <typename TaskType>
auto waitForTasks(TaskType&& aTask)
{
    return std::make_tuple(aTask->task_promise.get_future()); 
}

template <typename ShFutureType, UnaryFunction<result_t> FunType, UnaryFunction<result_t>... FunTypes>
auto makeTasksTuple(ShFutureType shFuture, FunType&& aFun, FunTypes&&... moreFuns)
{
    //recursive calls to traverse variadic template
    return std::tuple_cat(
    makeTasksTuple(shFuture, std::forward<FunType>(aFun)),
    makeTasksTuple(shFuture, std::forward<FunTypes>(moreFuns)...));
}

template <typename SharedFutureType, UnaryFunction<result_t> FuncType>
auto makeTasksTuple(SharedFutureType shFuture, FuncType&& aFunc)
{
    using retType = invoke_result_t <FuncType, result_t>;
    //share parent to keep alive the current task
    auto lambdaCallable = [fun = std::move(aFunc), parent = shared_from_this(), fut = shFuture]()mutable
    {
        fun(fut.get()); // blocking call for previous task ,
                        // only after the result is available, then only it can be passed to next job
    };
    auto tsk = make_shared<STask<retType>>(executor_, lambdaCallable); 
    return make_tuple(tsk); 
}

private : 
void execute_impl()
{
    if constexpr (is_same_v<result_t, void>)
    {
        cout << "inside execute_impl for void return type" << endl;
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
