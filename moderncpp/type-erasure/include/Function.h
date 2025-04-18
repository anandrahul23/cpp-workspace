#pragma once

#include <iostream>
#include <utility>



template <typename T> class Function; 

template<typename R, typename... Args> 
class Function<R(Args...)> 
{
    public:
    template <typename Callable> Function(Callable const& callable) : impl(std::make_unique<Model<Callable>>(callable)) {}
    R operator()(Args... args) const {
        return impl->invoke(std::forward<Args>(args)...);
    }

    Function(Function const& other) : impl(other.impl->clone()) {}

    Function& operator=(Function const& other) {
        //copy and swap idiom
        Function tmp(other);
        std::swap(impl, tmp.impl);
        return *this;
    }

    Function& operator=(Function&&) = default;
    Function(Function&&) = default;

    private: 
    struct Concept {
        virtual ~Concept() = default;
        virtual R invoke(Args... args) const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };
    template<typename Callable> struct Model : Concept {
        Model(Callable const& callable) : callable_(callable) {}
        R invoke(Args... args) const override {
            return callable_(std::forward<Args>(args)...);
        }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model<Callable>>(callable_);
        }
        Callable callable_;
    };
    std::unique_ptr<Concept> impl;
};


void testFunction()
{
    std::cout << "testFunction 1234" << std::endl;
}   
Function<int(int)> f = [](int x) { return x + 1; };






