#pragma once
#include <cstddef>
#include <type_traits>



namespace MyUtility
{
    template <int N>
    struct Fibonnaci
    {
        static_assert( N >=0, "fib can not be calulate for negative number");
        static constexpr long long val = Fibonnaci<N - 1>::val + Fibonnaci<N - 2>::val;
    };

    template <>
    struct Fibonnaci<0>
    {
        static constexpr long long  val{0};
    };

    template <>
    struct Fibonnaci<1>
    {
        static constexpr long long val{1};
    };

    template <typename T> 
    struct Remove_Reference
    {
        using type = T; 
    }; 

    template <typename T> 
    struct  Remove_Reference<T&> 
    {
        using type = T; 
    };
    template <typename T>
    struct Remove_Reference<T&&>
    {
        using type = T;
    }; 

    template <typename T>
    using Remove_Reference_t = typename Remove_Reference<T>::type;

    template <typename T>
    Remove_Reference_t<T>&& Move(Remove_Reference_t<T> &&arg)
    {
        return static_cast<Remove_Reference_t<T>&&>(arg); 
    }
}

