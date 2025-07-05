#pragma once
#include <iostream>
#include<thread>
#include <queue>
#include <atomic>
#include <random>
#include <type_traits> // For std::is_integral_v
#include <sstream>     // For std::stringstream

using namespace std;
//TODO make this work only for numeric data , which can be randomly generated
template <typename T> class ProducerConsumer
{
    public:
    ProducerConsumer() = default;
        ProducerConsumer(int capacity): buffer_capacity(capacity),
                                        engine(rd())
        {

            unsigned int max_threads = thread::hardware_concurrency();
            if (max_threads < 2)
            {
                max_threads = 2;
            }
            std::cout << "total hardware concurrency is:" << max_threads << endl;
            for(int i =0; i < max_threads/2; ++i)
            {
                threads_.emplace_back(thread{&ProducerConsumer::producer_loop, this});
                threads_.emplace_back(thread{&ProducerConsumer::consumer_loop, this}); 
            }
        }

        void producer_loop()
        {
            while(true)
            {
                // randomly generate some data and push to buffer 

                // //stop proucing more items when stop signal is received 
                if(stop_thread.load(memory_order_acquire))
                {
                    cout<<"stopping producing more items, stop signal received"<<endl;
                    cout<<"exiting producer thread with thread id#:"<<this_thread::get_id()<<endl;
                    break;
                }
               
                T data = dist(engine); 
                if(! produce(data))
                {
                    break;
                }
               
            }
        }

            ~ProducerConsumer()
        {
            producer_ready.notify_all();
            stop_thread.store(true, memory_order_release);

            consumer_ready.notify_all();

           

            for(auto& th: threads_)
            {
                if(th.joinable())
                {
                    th.join();
                }
            }
        }

        bool produce(const T &data)
        {
            {
            
                unique_lock<mutex> lk{mt_};
                producer_ready.wait(lk, [this]()
                                    { return bufferQueue.size() < buffer_capacity || stop_thread.load(memory_order_acquire); });

                
                // if(stop_thread.load(memory_order_acquire))
                // {
                //     cout<<"exiting producer therad in produce fn after stop signal with thread id#:"<<this_thread::get_id()<<endl;
                //     consumer_ready.notify_all();
                //     return false;
                // }   
                
                
                bufferQueue.push(data);
                cout << "thread with Thread id#" << this_thread::get_id() << " produced data:" << data << endl;
            }
        
            consumer_ready.notify_one();
            return true;
        }
    //TODO return data by value , for large data and preventing copy use shaared_ptr
        void consumer_loop()
        {
            while (true)
            {
                unique_lock<mutex> lk{mt_};
                consumer_ready.wait(lk, [this]()
                                    { return (!bufferQueue.empty() || stop_thread.load(memory_order_acquire)); });
                // when a thread reaches here , when buffer is empty 
                //it means only possible when stop_thread has become true
                /*stop_thread.load(memory_order_acquire) && */
                if(stop_thread.load(memory_order_acquire) &&   bufferQueue.empty())
                {
                    cout<<"exiting consumer thread with thread id#:"<<this_thread::get_id()<<endl;
                    consumer_ready.notify_all();
                    producer_ready.notify_all();
                    lk.unlock();
                    break;
                }
                if(stop_thread.load(memory_order_acquire))
                {
                    cout<<"Processing consumer thread after stop signal with thread id#:"<<this_thread::get_id()<<endl;
                }
                T tempData = bufferQueue.front();

                cout<<"Thread with Thread Id#:"<<this_thread::get_id()<<" consumed data:"<<tempData<<endl;

                bufferQueue.pop();
                lk.unlock();
                producer_ready.notify_one();
            }              
        }

    private: 

    queue <T> bufferQueue; 
    mutex mt_; 

    condition_variable producer_ready; 
    condition_variable consumer_ready;  
    int buffer_capacity =0;

    atomic<bool> stop_thread = false;
    random_device rd;
    // engine
    mt19937 engine;
    uniform_int_distribution<T> dist{};

    vector<thread> threads_;
}; 