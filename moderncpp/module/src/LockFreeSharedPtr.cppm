module;
#include <atomic>
#include <utility>
export module leonrahul.LockFreeSharedPtr;

using namespace std;

export namespace ThreadSafeWorld
{

    class ControlBlock
    {

    private:
        atomic<int> count;

    public:
        ControlBlock() : count{1} {};
        int addRef()
        {
            return count.fetch_add(1, std::memory_order_acq_rel);
        }
        int removeRef()
        {
            return count.fetch_sub(1, std::memory_order_acq_rel);
        }

        int getCount() const
        {
            return count.load(memory_order_acquire); 
        }
    };

    template <typename T>
    class LockFreeSharedPtr
    {
    private:
        ControlBlock *cb;
        T *ptr;

    public:
        LockFreeSharedPtr() : cb{nullptr}, ptr{nullptr} {};
        constexpr LockFreeSharedPtr(std::nullptr_t) : cb{nullptr}, ptr{nullptr} {};
        LockFreeSharedPtr(T *ptr) : cb{new ControlBlock()}, ptr{ptr}
        {
        };

        // copy Constructor
        LockFreeSharedPtr(const LockFreeSharedPtr& other)
        {
            cb = other.cb;
            ptr = other.ptr;
            if(cb)
            {
                cb->addRef();
            }
        }
        // copy Assignment
        LockFreeSharedPtr& operator=(const LockFreeSharedPtr& other)
        {
            if(this != &other)
            {
                LockFreeSharedPtr temp{other};
                this->swap(temp);
            }  
            return *this;
        }

        void swap(LockFreeSharedPtr& other)
        {
            std::swap(cb, other.cb);
            std::swap(ptr, other.ptr);
        }

        // Move constructor, default will not work here 
        //as the other will be holding the original ptrs , unless explicitly set to null ptr
        LockFreeSharedPtr(LockFreeSharedPtr&& other):cb(other.cb),ptr(other.ptr)
        {
            other.cb = nullptr;
            other.ptr = nullptr;
        }

        // Move assignment operator
        LockFreeSharedPtr &operator=(LockFreeSharedPtr&& other)
        {
            if(this != &other)
            {
                LockFreeSharedPtr temp{std::move(other)};
                this->swap(temp);
                //memory will be released after swapping this in temp 
                //and destructort called for temp while stack unwinding 
            }

            return *this;
            
        }
        

        ~LockFreeSharedPtr()
        {
            release();
        }

        void reset()
        {

            release();
            cb = nullptr;
            ptr = nullptr;

        }

        void reset(T *p)
        {
            release();
            cb = new ControlBlock();
            ptr = p;
        }   
        void
        release()
        {
            if (cb && cb->removeRef() == 1) // count now becomes 0
            {
                atomic_thread_fence(memory_order_acquire); // the acquire fence is required , beacuse all the writes must
                // be visble by all threads , we have memory order release for cnt sub in control block , and we need to ensure
                // visibility of all the writes before destruction only
                //  release the memory
                
                delete ptr;
                delete cb;
                cb = nullptr;
                ptr = nullptr;
            }
        }
        T& operator* () const
        {
            return *ptr;
        }

        T* operator-> () const
        {
            return ptr;
        }

        T* get() const
        {
            return ptr; 
        }
        explicit operator bool() const noexcept
        {
            return ptr != nullptr;
        } 

        int getCount() const
        {
            return cb->getCount();
        }

    };
}
