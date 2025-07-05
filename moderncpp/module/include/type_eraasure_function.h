
// #pragma once
// #include <memory>
// using namespace std;
// template <typename R, typename... Args>
// class MyFunction
// {



//     private: 
//     struct Concept
//     {
//         virtual ~Concept() = 0; 
//         R invoke(Args&&... args) =0; 
//         unique_ptr<Concept> clone() =0; 
//     }; 

//     template <typename CallableType>
//     struct Model : public Concept
//     {

//         Model(const Callable& callable): callable_(callable)
//         {

//         }
//         Model(Callable&& rValCallable): callable_(move(rValCallable))
//         {

//         }

//         R invoke(Args&&... args)
//         {
//             return callable_(forward(args...)); 
//         }

//     unique_ptr<Concept> clone()
//     {
//         return make_unique<Model<CallableType>>(callable_);
//     }

//     private : Callable callable_;
//     } 
//     template <typename Callable>
//     MyFunction(Callable &&callable) : concept_pimpl_(make_unique <Model<Callable>>(forward(callable)));

//     //implement copy constructor and copy assignment based on clone 
//     //default the move constructor and move assignment , beacuse of unique_ptr 

//     R operator()(Args... args) cosnt{
//         return concept_pimpl_->invoke(forward(args...)); 
//     }

//     unique_ptr<Concept> concept_pimpl_;

// public: 



// }; 