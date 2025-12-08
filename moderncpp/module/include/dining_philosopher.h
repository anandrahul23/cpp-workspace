// #pragma once

// #include <string>
// #include <array>
// #include <vector>
// #include <mutex>
// #include <semaphore>
// #include <condition_variable>
// #include <thread>
// #include <memory>
// #include <functional>


// using namespace std; 

// struct Chopstick
// {
//    mutex chop_mutex_; 

// };

// struct Philosopher
// {

//     void eat()
//     {

//     }

//     void idle()
//     {

//     }

//     void startAction()
//     {
//         currentState = [this]()
//                         {
//                             this->eat(); 
//                         }; 
//         mylife_([fun{currentState}]()
//                 {
//                     fun(); 
//                 });
//     }

//     std::string name:
//     std::string id; 

//     array<weak_ptr<Chopstick>, 2> chopsticks_; 

//     function<void()> currentState; 
//     jthread mylife_; 

// }; 
