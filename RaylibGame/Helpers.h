#pragma once
template<typename T, typename V>
struct is_of_type
{
    static constexpr bool value = false;
};

template<typename T>
struct is_of_type<T,T>
{
    static constexpr bool value = true;
};