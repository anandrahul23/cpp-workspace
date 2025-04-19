module;
#include <cstring>
#include <algorithm>
export module leonrahul.CustomStringArray;

export namespace StringWorld
{

    class CustomStringArray
    {

    private:
        char **stringArray_;
        int size_; // size of the array

    public:
        CustomStringArray() : stringArray_{nullptr}, size_{0} {};
        // Constructor
        CustomStringArray(char **arr, int s)
        {
            size_ = s;
            stringArray_ = new char *[s];
            for (int i = 0; i < s; ++i)
            {
                if (arr[i])
                {
                    int len = strlen(arr[i]) + 1; // +1 for '\0' char
                    stringArray_[i] = new char[len];
                    memcpy(stringArray_[i], arr[i], len);
                }
                else
                {
                    stringArray_[i] = nullptr;
                }
            }
        }
        // copy constructor
        CustomStringArray(const CustomStringArray &other)
        {
            this->size_ = other.size_;

            stringArray_ = new char *[size_];
            for (int i = 0; i < size_; ++i)
            {
                if (other.stringArray_[i])
                {
                    int len = strlen(other.stringArray_[i]) + 1;

                    this->stringArray_[i] = new char[len];

                    memcpy(stringArray_[i], other.stringArray_[i], len);
                }
                else
                {
                    stringArray_[i] = nullptr;
                }
            }
        }

        // copy assignment operator

        CustomStringArray &operator=(const CustomStringArray &other)
        {
            // a liitle optimization to check for self assignment and
            //  not needed to create a copy
            if (this != &other)
            {
                CustomStringArray temp{other}; // copy constructor called
                std::swap(*this, temp);        // swaps the pointer of this and size to temp , shallow copy;
            }
            return *this; // when stack unwindining , destructor for temp is called and memory is released
        }
        // destructor
        ~CustomStringArray()
        {
            release();
        }
        void release()
        {

            for (int i = 0; i < size_; ++i)
            {
                delete[] stringArray_[i];
            }

            delete[] stringArray_;
            stringArray_ = nullptr;
        }

        // move constructor
        CustomStringArray(CustomStringArray &&other)
        {
            this->size_ = other.size_;
            this->stringArray_ = other.stringArray_;
            other.size_ = 0;
            other.stringArray_ = nullptr;
        }

        // move assignment operator
        CustomStringArray &operator=(CustomStringArray &&other)
        {
            // a liitle optimization to check for self assignment and
            //  not needed to delete
            if (this != &other)
            {
                release();
                this->size_ = other.size_;
                this->stringArray_ = other.stringArray_;
                other.size_ = 0;
                other.stringArray_ = nullptr;
            }
            return *this;
        }
        int getSize() const
        {
            return size_;
        }
        char *get(int index) const
        {
            if (index < 0 || index >= size_)
            {
                return nullptr;
            }
            return stringArray_[index];
        }
        void add(const char *str)
        {
            char **temp = new char *[size_ + 1];
            for (int i = 0; i < size_; ++i)
            {
                temp[i] = stringArray_[i];
            }
            temp[size_] = new char[strlen(str) + 1];
            strcpy(temp[size_], str);
            delete[] stringArray_;
            stringArray_ = temp;
            size_++;
        }
    };
}
