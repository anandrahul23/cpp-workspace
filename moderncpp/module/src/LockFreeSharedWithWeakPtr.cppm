module;
#include <atomic>
#include <utility>
export module leonrahul.LockFreeSharedWithWeakPtr;

using namespace std;

export namespace ThreadSafeWorld
{
    class ControlBlock
    {
    private:
        atomic<int> use_count;
        atomic<int> weak_count;

    public:
        ControlBlock() : use_count{1}, weak_count{0} {};
        
        int addRef()
        {
            return use_count.fetch_add(1, std::memory_order_relaxed);
        }
        
        int removeRef()
        {
            return use_count.fetch_sub(1, std::memory_order_acq_rel);
        }

        int addWeakRef()
        {
            return weak_count.fetch_add(1, std::memory_order_relaxed);
        }

        int removeWeakRef()
        {
            return weak_count.fetch_sub(1, std::memory_order_acq_rel);
        }

        int use_count_val() const
        {
            return use_count.load(memory_order_acquire);
        }

        int weak_count_val() const
        {
            return weak_count.load(memory_order_acquire);
        }
    };

    template <typename T>
    class LockFreeSharedWithWeakPtr
    {
    private:
        atomic<ControlBlock*> cb{nullptr};
        atomic<T*> ptr{nullptr};

    public:
        LockFreeSharedWithWeakPtr() noexcept = default;
        constexpr LockFreeSharedWithWeakPtr(std::nullptr_t) noexcept : cb{nullptr}, ptr{nullptr} {};
        
        explicit LockFreeSharedWithWeakPtr(T* p) noexcept 
            : cb{p ? new ControlBlock() : nullptr}
            , ptr{p}
        {
            if(p) {
                atomic_thread_fence(std::memory_order_release);
            }
        }

        LockFreeSharedWithWeakPtr(const LockFreeSharedWithWeakPtr& other) noexcept
        {
            ControlBlock* other_cb = other.cb.load(std::memory_order_acquire);
            T* other_ptr = other.ptr.load(std::memory_order_acquire);
            
            if(other_cb)
            {
                other_cb->addRef();
                cb.store(other_cb, std::memory_order_release);
                ptr.store(other_ptr, std::memory_order_release);
            }
        }

        LockFreeSharedWithWeakPtr& operator=(const LockFreeSharedWithWeakPtr& other) noexcept
        {
            if(this != &other)
            {
                LockFreeSharedWithWeakPtr temp{other};
                this->swap(temp);
            }
            return *this;
        }

        void swap(LockFreeSharedWithWeakPtr& other) noexcept
        {
            ControlBlock* temp_cb = cb.load(std::memory_order_acquire);
            T* temp_ptr = ptr.load(std::memory_order_acquire);
            
            cb.store(other.cb.load(std::memory_order_acquire), std::memory_order_release);
            ptr.store(other.ptr.load(std::memory_order_acquire), std::memory_order_release);
            
            other.cb.store(temp_cb, std::memory_order_release);
            other.ptr.store(temp_ptr, std::memory_order_release);
        }

        LockFreeSharedWithWeakPtr(LockFreeSharedWithWeakPtr&& other) noexcept
        {
            ControlBlock* other_cb = other.cb.load(std::memory_order_acquire);
            T* other_ptr = other.ptr.load(std::memory_order_acquire);
            
            cb.store(other_cb, std::memory_order_release);
            ptr.store(other_ptr, std::memory_order_release);
            
            other.cb.store(nullptr, std::memory_order_release);
            other.ptr.store(nullptr, std::memory_order_release);
        }

        LockFreeSharedWithWeakPtr& operator=(LockFreeSharedWithWeakPtr&& other) noexcept
        {
            if(this != &other)
            {
                release();
                
                ControlBlock* other_cb = other.cb.load(std::memory_order_acquire);
                T* other_ptr = other.ptr.load(std::memory_order_acquire);
                
                cb.store(other_cb, std::memory_order_release);
                ptr.store(other_ptr, std::memory_order_release);
                
                other.cb.store(nullptr, std::memory_order_release);
                other.ptr.store(nullptr, std::memory_order_release);
            }
            return *this;
        }

        ~LockFreeSharedWithWeakPtr()
        {
            release();
        }

        void reset() noexcept
        {
            release();
            cb.store(nullptr, std::memory_order_release);
            ptr.store(nullptr, std::memory_order_release);
        }

        void reset(T* p) noexcept
        {
            if(p != ptr.load(std::memory_order_acquire))
            {
                release();
                cb.store(p ? new ControlBlock() : nullptr, std::memory_order_release);
                ptr.store(p, std::memory_order_release);
            }
        }

        void release() noexcept
        {
            ControlBlock* current_cb = cb.load(std::memory_order_acquire);
            if (current_cb && current_cb->removeRef() == 1)
            {
                atomic_thread_fence(std::memory_order_acquire);
                
                T* current_ptr = ptr.load(std::memory_order_acquire);
                delete current_ptr;
                
                if(current_cb->weak_count_val() == 0)
                {
                    delete current_cb;
                }
                cb.store(nullptr, std::memory_order_release);
                ptr.store(nullptr, std::memory_order_release);
            }
        }

        T& operator*() const noexcept
        {
            return *ptr.load(std::memory_order_acquire);
        }

        T* operator->() const noexcept
        {
            return ptr.load(std::memory_order_acquire);
        }

        T* get() const noexcept
        {
            return ptr.load(std::memory_order_acquire);
        }

        explicit operator bool() const noexcept
        {
            return ptr.load(std::memory_order_acquire) != nullptr;
        }

        int use_count() const noexcept
        {
            ControlBlock* current_cb = cb.load(std::memory_order_acquire);
            return current_cb ? current_cb->use_count_val() : 0;
        }

        int weak_count() const noexcept 
        {
            ControlBlock* current_cb = cb.load(std::memory_order_acquire);
            return current_cb ? current_cb->weak_count_val() : 0;
        }
    };
}
