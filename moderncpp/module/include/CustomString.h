#pragma once 
#include <cstddef>
#include <stdexcept> // For std::out_of_range
#include <ostream>

class CustomString
{
//declare all 7 and implement in .cpp file 
public:
    constexpr CustomString() : str{nullptr}, len { 0 }
    {

    }
    ~CustomString();
    CustomString(const char* other);
    CustomString(const CustomString& other);              // copy constructor
    CustomString& operator=(const CustomString& other);   // copy assignment operator
    CustomString(CustomString&& other);
    CustomString& operator=(CustomString&& other) noexcept;   
    // move assignment operator
    CustomString(const char* ptr1, const char* ptr2);
    CustomString(const char *other, size_t l);

    constexpr char &
    operator[](size_t index)
    {
        if (str == nullptr || index >= len) {
            throw std::out_of_range("CustomString: index out of bounds or string is null.");
        }
        return str[index];
    }

    constexpr const char& operator[](size_t index) const {
        if (str == nullptr || index >= len) {
            throw std::out_of_range("CustomString: index out of bounds or string is null.");
        }
        return str[index];
    }

    constexpr size_t size() const { return len; }
    constexpr const char* c_str() const { return str; }

    //implement some operator 

    constexpr const char* operator *() const
    {
        return str; 
    }

    friend std::ostream& operator<<(std::ostream& os, CustomString& val); 
    //++ shift from left 
    //implement as data memebr , prefix 
    constexpr CustomString& operator++()
    {
        //shift by one char 
       if (len > 0)
       {
        memmove(str, str + 1, len);
        --len;
       }

        return *this; 
    }

    CustomString operator++(int)
    {
        CustomString res{*this}; //create a copy
        ++(*this); 
        return res; 
    }

    friend CustomString operator+(const CustomString& first, const CustomString& second); 
    

private: 
    void copy(const char* other); 
    void destroy();
    char* str;
    size_t len; // Store the length for efficiency
}; 