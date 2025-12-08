#include <iostream>
#include <type_traits>
#include <functional>
using namespace std;

template <typename T, T val>
struct IntegralConstant
{
    using type = T;
    constexpr static T value = val;

    operator T()
    {
        return val;
    }
};

using TrueType = IntegralConstant<bool, true>;
using FalseType = IntegralConstant<bool, false>;

template <bool val, typename T = void>
struct EnableIf
{
};

template <typename T>
struct EnableIf<true, T>
{
    using type = T;
};
template <bool val, typename T>
using EnableIf_t = typename EnableIf<val, T>::type;
template <typename T, typename FunType, typename... ArgsType>
auto funCall(FunType &&f, ArgsType... args) -> EnableIf_t<is_same_v<T, invoke_result_t<FunType, ArgsType...>>, T>
{
    return std::invoke(f, args...);
}

void foo1()
{
    cout << "foo1 called" << endl;
}

int foo2()
{
    cout << "foo2 called" << endl;
    return 0;
}
