#pragma once 


// Define addition of type in a pack of list like tuples , variant 


template <typename TypeListT, typename NewType> 
struct Push_Back_Type; //no definition 

//partial sepcialization for a list of types

template <template<typename...> class TypeListT, typename... ElementType, typename NewType>
struct Push_Back_Type<TypeListT<ElementType...>, NewType>
{
    using type = TypeListT<ElementType..., NewType>;
};

template <typename TypeListT, typename NewType>
using Push_Back_Type_t = typename Push_Back_Type < TypeListT, NewType>::type;


template <typename TypeListT>
struct Remove_Front_type;

template <template <typename...> class TypeListT, typename FrontElType, typename... ElementTypes>
struct Remove_Front_type<TypeListT<FrontElType, ElementTypes...>>
{
    using type = TypeListT<ElementTypes...>;
};

template<template <typename...> class TypeListT> 
struct Remove_Front_type<TypeListT<>>
{
    using type = TypeListT<>;
};

template <typename TypeListT>
using Remove_Front_type_t = typename Remove_Front_type<TypeListT>::type;

// template<typename ContainerType, template <typename...> AnotherContainerType>
// struct RenameContainerType;

// template <template <class...> class ContainerType, template <class...> class AnotherContainerType, class... Elements>
// struct RenameContainerType<ContainerType<Elements...>, AnotherContainerType<Elements...>>
// {
//     using type = AnotherContainerType<Elements...>;
// };

// // A helper alias for easier use
// template <class ContainerType, class OldListType>
// using RenameContainerType_t = typename RenameContainerType<ContainerType, OldListType>::type;
