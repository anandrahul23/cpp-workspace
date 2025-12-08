// #pragma once
// #include <string>
// #inclue <priority_queue>
// #include <map>
// #include <chrono> 

// using namespace std; 
// struct Value
// {

// }; 
// struct ValueWithTime
// {
//     Value val; 
//     string key; 
//     //time point 
//     std::chrono Timepoint time_point;

//     bool operator==(const ValueWithTime& other)
//     {
//         return  other.time_point == time_point; 
//     }
//     bool operator<(const ValueWithTime &other)
//     {
//         return other.time_point > time_point;
//     }

// }
// namespace std
// {
//     template<typename T> 
//     class Hash<ValueWithTime> 
//     {
//         public: 
//          Hash  operator(){
//              return hash<Value>{}(val) ^ 1 && hash<Timepoint>{}(time_point); 
//          }

//     }; 

// }

// class Cache 
// {
 
//     public:
//     void put(std::string key, Value* val)
//     {
//         if(!checkSize())
//         {
//             auto it = map_cache_.find(key);
//             if (it == map_cache_.end())
//             {
//                auto topEl =  minHeap.top(); 
                
//               if(auto it =  map_cache_.find(topEl.key); it !=map_cache_.end()) 
//               {
//                   map_cache_.erase(it);
//                   minHeap.pop();
//               }
//               map_cache_[key] = *val;
//               minHeap.push(ValueWithTime{key, *val, std::chrono::steady_clock::now()});
//             }
//         }
//         else
//         {
//             auto it = map_cache_.find(key); 
//             if(it == map_cache_.end())
//             {
//                 map_cache_.insert(make_pair(key, *val)); 
//                 minHeap.push(ValueWithTime{key, *val, std::chrono::steady_clock::now()}); 
//             }
//         }

        
        
//     }

//     bool checkSize()
//     {
//        if (map_cache_.size() >=100)
//         return false; 
//        return true; 
//     }
//     Value * get(const std::string &  key)
//     {
//         auto it = map_cache_.find(key);
//         if(it != map_cache_.find())
//         {
//             minHeap.push(ValueWithTime{key, it->second, std::chrono::steady_clock::now()});
//             //minHeap.erase
//             //old key is not clear 

//             return &it->second; 
//             //heapify()
//         }
//         return nullptr;
//     }

//     private:
//         map<stirng, Value> map_cache_;
//         priority_queue<ValueWithTime> minHeap 

// }; 
// 1 2 , 3, 4 




// //#################

// 0, 1, 1, 2, 3, 5 , 8 , 13 



template < typename T> constexpr  auto Fib(const T &num)
{
    
    if constexpr (is_same_v <integral_constant<num>, integral_constant<0>> || is_same_v <integral_constant<num>, integral_constant<1>>)
    {
        return num; 
    }
    return Fib(num -1) + Fib(num - 2);
}

// int Fib(int num)

// {
    
//     if(num == 1 && num ==2)
//     {
//         return num -1; 
//     }
    
//     return Fib(num-1) + Fib(num-2); 
// }
// 0, 1, 2, 3, 4 
// 5
// Fib(4) , Fib(3)
// Fib(3), Fib(2) -> Fib(2) , Fib(1)
// 1


// th1 , 1 , th2 2, th3 3 



// vector<thread> threads; 
// mutex mu_; 
// static int counter {1}; 
// condition_variable consumer_ready; 
// atomic<bool> flag_; 
// atomic<int> counter; 

// class Counter
// {
//     static thread_local int counter; 


// }

// void producer()

// {


// }
// void print_integer(int index)
// {
//     while(true
//     )
//     {

//         scoped_lock<mutex>{mu_};
//         if(counter % index == 0)
//         {
//             cout<<counter; 
//         }
//         ++counter; 
        

//     }
// }

// for(int i =1; i<=3; ++i)
// {
//     threads.emplace_back(print_integer, i); 
// }


// //###################
// template<typename T>
// class MyIterator; 

// template<typename T> 
// class LinkedList 
// {

//     public: 
//     using iterator = MyIterator<T>;
//     uisng const_iteraor = MyIterator<const T>;

//     itertaor begin()
//     {
//         MyIterator<T> it(head, size_);
//         return it.begin(); 
//     } 
//     const_iterator begin() const; 
//     const_iteraator cbegin(); 

// };

// template <typename T>
// class MyIterator

// {

//     using value_type = T; 
//     using reference = T&;
//     using pointer = T*; 
//     using iterator_category = forward_iterator_tag; 
    
//     public: 
//     Myiterator(): nullptr{}
//     Myiterator(T* val, size_t sz):val_(val), length(sz){}

//     Myiterator begin()
//     {
//         return MyIterator(val_); 
//     }

//     const Myiterator begin() const
//     {
//         return MyIterator(val_);
//     }

//     Myiterator end()
//     {
//         return MyIterator(val_+length);
//     }

//     T& operator*()
//     {
//         return *val_; 
//     }
//     T* operator->()
//     {
//         val_;
//     }

//     //prefix 
//     MyIterator& opreator++()
//     {
//         ++val_; 
//         return *this; 
//     }
//     //postfix
// MyIterator opreator++(int)
// {
//     MyIterator temp{*this}; 
//     ++(*this); 
//     return temp; 
// }

//     private : T *val_;
//     size_t length; 
// }; 

// concept
// range 
// views 
// coroutine 



// auto fun ()

// {
//     return ... 
// }

// auto variable = fun(); 
// declytype(variable) anotherVar 

// void fun(string& v)

// string str{"abd"}

// thread t(fun, std::ref(str));

// struct ABC
// {
//     operator() (const string .......) 
//     {

//     }
// }abc; 
// abc();

// function <void(string)> someFn = [&lambdaFn]() 
// {
// }

//     lambdaFn = [&str](const String &aStr)
// {
//     return str
// };

// atomic<bool> stop_flag{false} ; 

// stop_flag.store(true, memory_order_release)

// stop_flag.load(memory_order_acquire)




