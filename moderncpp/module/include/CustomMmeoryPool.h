#pragma once 

#include <memory>
#include <cstddef>
#include <utility>
#include <exception>
using namespace std;

template <typename T> 
class CustomeMemoryPool
{

    public: 
    CustomeMemoryPool(size_t slabSize = 256): slabSize_{slabSize}  
    {
        addSlab(); 
    }
    template<typename... ArgsType>
    T* create(ArgsType&&... args)
    {
        cout<<"create called"<<endl;
        if(!freeHead_)
        {
            addSlab(); 
        }

        Node* cur = freeHead_; 
        freeHead_ = freeHead_->next; 
        void* curMemory =  static_cast<void*>(cur->nodeStorage);
        try
        {
            T* obj = ::new(curMemory) T(std::forward<ArgsType>(args) ...);
            return obj;
        }
        catch(...)
        {
            //rearrange the freeHead_ 
            std::exception_ptr exPtr = std::current_exception();
            cout << "exception caught while creating object from pool"<<endl;
            cur->next = freeHead_;
            freeHead_ = cur;
            throw; 
        }
       
        
    }
    void handle_eptr(std::exception_ptr eptr) // passing by value is OK
    {
        try
        {
            if (eptr)
                std::rethrow_exception(eptr);
        }
        catch(const std::exception& e)
        {
            std::cout << "Caught exception: '" << e.what() << "'\n";
        }
    }

    void destroy(T* ptr)
    {
        cout << "detroy called" << endl;
        ptr->~T(); 
        //add it to freeHead_ 
        Node* n = reinterpret_cast<Node*>(reinterpret_cast<void*>(ptr)); 
        n->next = freeHead_; 
        freeHead_ = n;
        cout << "detroy end" << endl;
    }

    private: 

    alignas(T) union Node
    {
        Node* next; 
        std::byte nodeStorage[sizeof(T)]; 
    }; 

    Node* freeHead_{nullptr}; 

    void addSlab()
    {
        auto slab = unique_ptr<Node[]>(new Node[slabSize_]);
        for(size_t i = 0; i < slabSize_; ++i)
        {
            Node& n = slab[i]; 
            n.next = freeHead_; 
            freeHead_ = &n; 
        }
        memoryBuffer_.push_back(std::move(slab)); 
    }

    vector<unique_ptr<Node[]>> memoryBuffer_; 
    size_t slabSize_;

}; 