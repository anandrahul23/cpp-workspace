#pragma once
#include <memory>

	template <typename T, size_t N> constexpr size_t dimensionnOf(const T (&)[N]) {
		return N;
	}



//variadatic template


template <typename... Ts, typename T>
void fooVariadic(T value, Ts... values)
{
	std::cout << sizeof...(Ts);
	std::cout<<"val:"<<value<<std::endl;
	if constexpr (sizeof...(Ts) > 0)
	{
		fooVariadic(values...);
	}
}

template <typename T> void fooVariadic(T value)
{
	std::cout<<"val:"<<value<<std::endl;
}

//Create a custom Print for any data type

void printVariadicTailRec ()
{
}
	
template <typename T, typename... Ts>
void printVariadicTailRec(T val, const Ts... values)
{
	std::cout<<val<<endl;
	printVariadicTailRec(values...);
}

template<typename T, typename... Ts> 
void printConstExprMode(const T& val, const Ts&... values)
{
	std::cout<<val<<endl;
	if constexpr(sizeof... (Ts) > 0)
	{
		printConstExprMode(values...);
	}
}

template <typename... Ts> 
void printFold(const Ts&... values)
{
	using namespace std;
	((cout<<" "<<values), ...);
	cout<<endl;
	//lets use some lambda for the fold 
	auto lambdaPrint = [](auto const& val){
		cout<<"***"<<val<<"***"<<endl;
	};
	(lambdaPrint(values), ...);
	cout<<endl;
}

template <typename T, typename... Ts> 
auto MaxWithFold(const T& val1, const Ts&... values)
{
	auto max_value = val1; 

	( (max_value = max_value > values ? max_value : values ), ...); //applied fold operatin

	return max_value; 
}

//practice type traits 

template <typename T, T v>
struct IntegralConstant
{
	using value_type = T; 
	using type = IntegralConstant<T, v>; 
	constexpr static T value = v; 
	constexpr operator value_type() const noexcept
	{
		return v; 
	}

}; 

template <bool b> 
using BooleanConstant = IntegralConstant<bool, b>;

using true_type = BooleanConstant<true>; // IntegralConstant<bool, true>
using false_type = BooleanConstant<false>; //IntegralConstant<bool, false>

template<bool B, typename T = void> 
struct EnableIf
{
};

template<typename T>
struct EnableIf<true, T> 
{
	using type = T;
};
template <bool b, typename T = void>
using EnableIf_t = EnableIf<b, T>::type; 
