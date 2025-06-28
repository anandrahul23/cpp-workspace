#include "../include/CustomString.h"

#include <cstring>
#include <utility>
#include <stdexcept>


CustomString::CustomString(const char* other): str{nullptr}, len{0}
{
    copy(other);
}

void CustomString::copy(const char* other)
{
    if (other != nullptr)
    {
        len = strlen(other);
        str = new char[len + 1]; //+1 for '\0' character
        // strcpy(str, other);
        // using memcopy for bitwise copy
        memcpy(str, other, len + 1);
    }
    else
    {
        str = nullptr;
        len = 0;
    }
}

CustomString::CustomString(const CustomString& other)
{
    if(other.str != nullptr)
    {
        copy(other.str);
    }
    else
    {
        str = nullptr;
        len = 0;
    }
}

CustomString::~CustomString()
{
    destroy();
}

void CustomString::destroy()
{
    if (nullptr != str)
    {
        delete[] str;
        len = 0;
    }
    str = nullptr;
}

CustomString& CustomString::operator=(const CustomString& other) // let compiler make the copy
{
    // copy swap idiom
    CustomString temp{other};
    std::swap(temp.str, this->str);
    std::swap(temp.len, this->len);

    //when temp goes out of scope previous str's memory will be released
    //It handles self assignment also by itself , but we can add a check 
    return *this; 
}

CustomString::CustomString(CustomString&& other)
{
    str = other.str;
    len = other.len;
    //make other str point to nullptr
    other.str = nullptr;
    other.len = 0;
}

CustomString& CustomString::operator=(CustomString&& other) noexcept
{
    if (this != &other) { // Self-assignment check
        destroy(); 
        str = other.str; 
        len = other.len;
        other.str = nullptr; 
        other.len = 0;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, CustomString &val)
{
    //make sure point is not null 
    if(val.str == nullptr)
    {
        throw std::runtime_error{"null pointer"};
    }
    os<<val.str; 
    return os; 
}

CustomString::CustomString(const char* ptr1, const char* ptr2)
{
    // Treat null pointers as empty strings for robustness.
    const char *p1 = (ptr1 != nullptr) ? ptr1 : "";
    const char *p2 = (ptr2 != nullptr) ? ptr2 : "";
    size_t len1 = strlen(p1);
    size_t len2 = strlen(p2);
    size_t lenLocal = len1 + len2;

    if (lenLocal > 0)
    {
        str = new char[lenLocal + 1];
        memcpy(str, p1, len1);
        memcpy(str + len1, p2, len2 + 1); // Copy p2 and its null terminator
    }
    else
    {   
        str = nullptr;
    }
}

CustomString::CustomString(const char *ptr, size_t l)
{
    if (ptr != nullptr)
    {
        
        str = new char[l]; //+1 for '\0' character
        // strcpy(str, other);
        // using memcopy for bitwise copy
        memcpy(str, ptr, l);
    }
    else
    {
        str = nullptr;
        len = 0;
    }
}

CustomString operator+(const CustomString &first, const CustomString &second)
{
    if(nullptr == first.str || nullptr == second.str)
    {
        throw std::logic_error{"one of the string has null ptr"};
    }
    return CustomString{first.str, second.str};
}