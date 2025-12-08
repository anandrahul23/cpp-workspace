#pragma once 
#include <string> 
#include <iostream>
#include <atomic>

using namespace std; 

struct Counter
{

    long long incAndGet()
    {
        ++cnt; 
        return cnt; 
    }
    long long getCnt()
    {
        return cnt; 
    }
    long long cnt{0}; 
};

struct AtomicRleaxedCounter
{

    long long incAndGet()
    {
        cnt.fetch_add(1, memory_order_relaxed);
        return cnt;
    }
    long long getCnt()
    {
        return cnt.load(memory_order_relaxed);
    }
    atomic<long long> cnt{0};
};

void runMillionIncrement(Counter& normalCounter, string message)
{
    for (int i = 0; i < 1000000; ++i)
    {
        normalCounter.incAndGet(); 
    }
    std::cout<<message<<"-> counter:"<<normalCounter.getCnt()<<endl;
}

void runAtomicRelaxedMillionIncrement(AtomicRleaxedCounter &atomicCounter, string message)
{
    for (int i = 0; i < 1000000; ++i)
    {
        atomicCounter.incAndGet();
    }
    std::cout << message << "-> counter:" << atomicCounter.getCnt() << endl;
}