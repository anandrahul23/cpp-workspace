#include "../include/ctasks.h"
#include "../include/producer_consumer.h"
#include "../include/ex_templates.h"   
#include "../include/ex_crtp.h"
#include "../include/thread_pool_using_packaged_task.h"
#include "../include/ExecutableTask.h"
#include "../include/ExecutorThreadPool.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <string>
#include <random>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <ranges>



using namespace std::chrono_literals;
using namespace std;

void print_thread_id()
{
    std::cout << "Current thread ID: " << std::this_thread::get_id() << std::endl;
}

ctask<int> test_coro()
{
    print_thread_id();
    co_return 42;
}

ctask<int> multiply(int a, int b)
{
    co_await "Multiply";
    co_return a *b;
}

ctask<int> mul_add(int a, int b, int c, int d)
{
    co_await "Mul_Add";
    auto p1 = multiply(a, b);
    co_return co_await multiply(c, d) + co_await p1;
}

template <typename iterator_t>
ctask<std::vector<double>> find_above_average(iterator_t from, iterator_t to, double average)
{
    co_await "ChunkAboveAverage";
    using namespace std::chrono_literals;
    std::vector<double> above_average;
    std::copy_if(from, to,
                 std::back_inserter(above_average),
                 [average](auto price)
                 {
                     std::this_thread::sleep_for(2s);
                     return price > average;
                 });

    co_return above_average;
}

ctask<int> fork_join_example()
{
    co_await "Root";
    vector<double> daily_price = {100.3, 101.5, 99.2, 105.1, 101.93,
                                  96.7, 97.6, 103.9, 105.8, 101.2};
    auto average = co_await [&daily_price]() -> ctask<double>
    {
        co_await "Average";
        auto average = 0.0;
        for (auto p : daily_price)
            average += p;
        average /= daily_price.size();
        co_return average;
    }();

    auto stddev_task = [&daily_price](double average) -> ctask<double>
    {
        co_await "StdDev";
        auto sum_squares = 0.0;
        for (auto price : daily_price)
        {
            auto distance = price - average;
            sum_squares += distance * distance;
        }
        co_return sqrt(sum_squares / (daily_price.size() - 1));
    }(average);

    auto above_average_task = [&daily_price](double average) -> ctask<vector<double>>
    {
        co_await "AboveAverage";
        vector<double> above_average;
        const auto nof_chunks = 4; // parallelism level
        const auto chunk_size = daily_price.size() / nof_chunks;

        auto start = chrono::steady_clock::now();

        auto from = daily_price.begin();
        auto to = from + chunk_size;
        vector<ctask<vector<double>>> tasks;
        for (auto i = 0; i < nof_chunks; ++i)
        {
            if (i == nof_chunks - 1)
                to = daily_price.end();
            tasks.push_back(find_above_average(from, to, average));
            from = to;
            to += chunk_size;
        }

        for (auto &t : tasks)
        {
            auto task_result = co_await t;
            above_average.insert(above_average.end(), task_result.begin(), task_result.end());
        }

        cout << "Elapsed time in seconds: "
             << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count()
             << endl;

        co_return above_average;
    }(average);

    cout << "Standard deviation: " << co_await stddev_task << endl;
    cout << "Elements above average: " << (co_await above_average_task).size() << endl;
    co_return 0;
}

int main(int argc, char *argv[])
{
    //return fork_join_example().get();

    // ProducerConsumer<int> samplePc(1000);
    // this_thread::sleep_for(1s);

   // MyMax(12, -43U);

    //printVariadicTailRec(1, "ranand", "hello world");
    // printFold("i", "am", "going", "to", "be", "very", "humble", "in life");

    // auto maxEl = MaxWithFold(5, 10, 98.098, 15.0F, 2L, 400U);
    // std::cout<<MaxWithFold(5)<<std::endl;

    // std::cout<< maxEl<<endl;
    // cout<<" size of strd::string:"<<sizeof(std::string)<<endl;

    //algorithm
    // std::vector<int> v{1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1};
    // auto it = std::search_n(v.cbegin(), v.cend(), 2, 0);
    // if (it != v.cend())
    //     std::cout << "found at index "
    //               << std::distance(v.cbegin(), it) << '\n';

    // unique_ptr<Circle> circlePtr = make_unique<Circle>();
    // unique_ptr<Circle> circlePtr1 = circlePtr->clone(); 
    // circlePtr->draw();
    // circlePtr1->draw();

    // ThreadPool pool{6}; 

    auto lamdaLargeCal = [] (const string& message, size_t noElem) 
                        {
                            cout<<"random integer generation thread id:"<<this_thread::get_id()<<endl;
                            random_device rd;
                            mt19937 engine(rd()); 
                            uniform_int_distribution<int> intDist(1, 10000);
                            cout<<message<<endl; 
                            vector<int> intVector;
                            intVector.reserve(noElem);
                            generate_n(back_inserter(intVector), 1000, [&intDist, &engine]() 
                                                                        {return intDist(engine);
                                                                        });
                            ostream_iterator<int> opItr(cout, " ");
                             ranges::copy(intVector, opItr);
                             return intVector; 
                        };

    cout<<"Main thread id:"<<this_thread::get_id()<<endl;

    // pool.scheduleTask(lamdaLargeCal, "generating 1000 random integers in a new thread", 1000); 

    ExeccutableThreadPool threadPool{6};

    auto myTask = globalScheduleTask(threadPool, lamdaLargeCal, "generating 1000 random integers in a new thread", 1000);
    auto res = myTask->then([](const vector<int>& intVec){

        //do some tramsform
        vector<int> resVec;
        cout<<"\n\n*****transforming the input vector to multiply by 2******\n\n"<<endl<<endl;
        ranges::transform(intVec, back_inserter(resVec), [](int num)
                          { return 2 * num; });
        return resVec; 
        
    })->get_future().get(); 

    ostream_iterator<int> opIt1(std::cout, "  ");
    ranges::copy(res, opIt1);
    return 0;
}
