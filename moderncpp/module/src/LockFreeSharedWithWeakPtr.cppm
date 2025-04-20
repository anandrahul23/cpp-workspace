module;
#include <atomic>
#include <utility>
#include <memory>
#include <functional>
export module leonrahul.LockFreeSharedWithWeakPtr;

using namespace std;

// Move to global namespace since we need specialization
template<typename T> struct hash;

export namespace ThreadSafeWorld {
    template<typename T> class LockFreeWeakPtr;  // Forward declaration
    template<typename T, typename Deleter, typename Allocator> class ControlBlockWithDeleter;
    template<typename T, typename Allocator> class ControlBlockMakeShared;

    // Base control block with virtual destructor
    class ControlBlock {
    private:
        atomic<int> use_count{1};
        atomic<int> weak_count{0};

    public:
        virtual ~ControlBlock() = default;
        virtual void destroy_object() noexcept = 0;
        virtual void destroy_this() noexcept = 0;

        void addRef() noexcept
        {
            use_count.fetch_add(1, memory_order_relaxed);
        }

        int removeRef() noexcept
        {
            return use_count.fetch_sub(1, memory_order_release);
        }

        void addWeakRef() noexcept
        {
            weak_count.fetch_add(1, memory_order_relaxed);
        }

        void removeWeakRef() noexcept
        {
            weak_count.fetch_sub(1, memory_order_release);
        }

        int use_count_val() const noexcept
        {
            return use_count.load(memory_order_acquire);
        }

        int weak_count_val() const noexcept
        {
            return weak_count.load(memory_order_acquire);
        }
    };

    // Basic control block for raw pointer case
    template<typename T>
    class BasicControlBlock : public ControlBlock {
        T* ptr;
    public:
        explicit BasicControlBlock(T* p) : ptr(p) {}
        
        void destroy_object() noexcept override {
            delete ptr;
        }
        
        void destroy_this() noexcept override {
            delete this;
        }
    };

    template<typename T, typename Allocator>
    class ControlBlockMakeShared : public ControlBlock {
        alignas(T) unsigned char storage[sizeof(T)];
        T* ptr_;
        [[no_unique_address]] Allocator alloc;
    
    public:
        template<typename... Args>
        explicit ControlBlockMakeShared(const Allocator& a, Args&&... args)
            : alloc(a) {
            ptr_ = new (storage) T(std::forward<Args>(args)...);
        }

        T* get_ptr() noexcept { return ptr_; }

        void destroy_object() noexcept override {
            if (ptr_) {
                ptr_->~T();
                ptr_ = nullptr;
            }
        }

        void destroy_this() noexcept override {
            using CBAllocType = typename std::allocator_traits<Allocator>::
                template rebind_alloc<ControlBlockMakeShared>;
            CBAllocType cb_alloc(alloc);
            this->~ControlBlockMakeShared();
            cb_alloc.deallocate(this, 1);
        }
    };

    template<typename T, typename Deleter, typename Allocator>
    class ControlBlockWithDeleter : public ControlBlock {
        T* ptr;
        [[no_unique_address]] Deleter deleter;
        [[no_unique_address]] Allocator alloc;
        
    public:
        ControlBlockWithDeleter(T* p, Deleter d, const Allocator& a) 
            : ptr(p), deleter(std::move(d)), alloc(a) {}

        void destroy_object() noexcept override {
            if (ptr) {
                deleter(ptr);
                ptr = nullptr;
            }
        }

        void destroy_this() noexcept override {
            using CBAllocType = typename std::allocator_traits<Allocator>::
                template rebind_alloc<ControlBlockWithDeleter>;
            CBAllocType cb_alloc(alloc);
            this->~ControlBlockWithDeleter();
            cb_alloc.deallocate(this, 1);
        }
    };

    // Add array support
    template<typename T>
    class ControlBlockArray : public ControlBlock {
        T* ptr;
        size_t size;
        
    public:
        explicit ControlBlockArray(size_t n) : size(n) {
            ptr = new T[n]();
        }
        
        void destroy_object() noexcept override {
            delete[] ptr;
        }
        
        void destroy_this() noexcept override {
            delete this;
        }
        
        T* get() noexcept { return ptr; }
    };

    template <typename T>
    class LockFreeSharedWithWeakPtr
    {
        // Allow LockFreeWeakPtr to access our internals
        friend class LockFreeWeakPtr<T>;
        template<typename U> friend class LockFreeSharedWithWeakPtr;

    private:
        struct alignas(16) PointerPair
        {
            ControlBlock *cb;
            T *ptr;
            bool operator==(const PointerPair &other) const noexcept
            {
                return cb == other.cb && ptr == other.ptr;
            }
        };

        // Separate atomic pointers for fallback mechanism
        struct AtomicPointers
        {
            atomic<ControlBlock *> cb{nullptr};
            atomic<T *> ptr{nullptr};
            atomic<bool> in_progress{false};
        };

        union
        {
            atomic<PointerPair> ptrs;
            AtomicPointers fallback;
        };

        static constexpr bool has_native_dwcas()
        {
            return atomic<PointerPair>::is_always_lock_free;
        }

        bool try_update_pointers(ControlBlock *new_cb, T *new_ptr)
        {
            if constexpr (!has_native_dwcas())
            {
                while (true)
                {
                    bool expected = false;
                    if (!fallback.in_progress.compare_exchange_strong(
                            expected, true, memory_order_acquire))
                    {
                        continue;
                    }

                    fallback.cb.store(new_cb, memory_order_relaxed);
                    fallback.ptr.store(new_ptr, memory_order_relaxed);

                    fallback.in_progress.store(false, memory_order_release);
                    return true;
                }
            }
            return false;
        }

        bool compare_exchange_ptrs(PointerPair &expected, const PointerPair &desired) noexcept
        {
            if constexpr (has_native_dwcas())
            {
                return atomic_compare_exchange_strong_explicit(
                    &ptrs, &expected, desired,
                    memory_order_acq_rel, memory_order_acquire);
            }
            else
            {
                while (true)
                {
                    auto current_cb = fallback.cb.load(memory_order_acquire);
                    auto current_ptr = fallback.ptr.load(memory_order_acquire);

                    if (current_cb != expected.cb || current_ptr != expected.ptr)
                    {
                        expected.cb = current_cb;
                        expected.ptr = current_ptr;
                        return false;
                    }

                    if (try_update_pointers(desired.cb, desired.ptr))
                    {
                        return true;
                    }
                }
            }
        }

        PointerPair load_ptrs(memory_order order) const noexcept
        {
            if constexpr (has_native_dwcas())
            {
                return ptrs.load(order);
            }
            else
            {
                while (fallback.in_progress.load(memory_order_acquire))
                {
                }
                return PointerPair{
                    fallback.cb.load(order),
                    fallback.ptr.load(order)};
            }
        }

        void store_ptrs(const PointerPair &new_ptrs, memory_order order) noexcept
        {
            if constexpr (has_native_dwcas())
            {
                ptrs.store(new_ptrs, order);
            }
            else
            {
                try_update_pointers(new_ptrs.cb, new_ptrs.ptr);
            }
        }

    public:
        using element_type = T;
        using weak_type = LockFreeWeakPtr<T>;

        LockFreeSharedWithWeakPtr() noexcept
        {
            if constexpr (has_native_dwcas())
            {
                ptrs = PointerPair{nullptr, nullptr};
            }
        }

        constexpr LockFreeSharedWithWeakPtr(std::nullptr_t) noexcept
        {
            if constexpr (has_native_dwcas())
            {
                ptrs = PointerPair{nullptr, nullptr};
            }
        }

        explicit LockFreeSharedWithWeakPtr(T *p) noexcept
        {
            if (p) {
                auto cb = new BasicControlBlock<T>(p);
                store_ptrs(PointerPair{cb, p}, memory_order_release);
            }
        }

        template<typename Deleter = default_delete<T>, typename Allocator = allocator<T>>
        LockFreeSharedWithWeakPtr(T* ptr, Deleter d = Deleter(), 
                                 const Allocator& alloc = Allocator()) {
            if (ptr) {
                using CBAllocType = typename allocator_traits<Allocator>::
                    template rebind_alloc<ControlBlockWithDeleter<T, Deleter, Allocator>>;
                CBAllocType cb_alloc(alloc);
                auto* cb = cb_alloc.allocate(1);
                new (cb) ControlBlockWithDeleter<T, Deleter, Allocator>(ptr, std::move(d), alloc);
                store_ptrs(PointerPair{cb, ptr}, memory_order_release);
            }
        }

        template<typename U>
        LockFreeSharedWithWeakPtr(const LockFreeSharedWithWeakPtr<U>& other, T* ptr) noexcept {
            auto other_ptrs = other.load_ptrs(memory_order_acquire);
            if (other_ptrs.cb) {
                other_ptrs.cb->addRef();
                store_ptrs(PointerPair{other_ptrs.cb, ptr}, memory_order_release);
            }
        }

        bool owner_before(const LockFreeSharedWithWeakPtr& other) const noexcept {
            return load_ptrs(memory_order_acquire).cb < other.load_ptrs(memory_order_acquire).cb;
        }

        LockFreeSharedWithWeakPtr(const LockFreeSharedWithWeakPtr &other) noexcept
        {
            auto other_ptrs = other.load_ptrs(memory_order_acquire);
            if (other_ptrs.cb)
            {
                other_ptrs.cb->addRef();
                store_ptrs(other_ptrs, memory_order_release);
            }
        }

        LockFreeSharedWithWeakPtr &operator=(const LockFreeSharedWithWeakPtr &other) noexcept
        {
            if (this != &other)
            {
                auto new_ptrs = other.load_ptrs(memory_order_acquire);
                if (new_ptrs.cb)
                {
                    new_ptrs.cb->addRef();
                }

                auto old_ptrs = exchange_ptrs(new_ptrs);
                if (old_ptrs.cb)
                {
                    release(old_ptrs);
                }
            }
            return *this;
        }

        void swap(LockFreeSharedWithWeakPtr &other) noexcept
        {
            auto my_ptrs = load_ptrs(memory_order_acquire);
            auto other_ptrs = other.load_ptrs(memory_order_acquire);

            while (!compare_exchange_ptrs(my_ptrs, other_ptrs) ||
                   !other.compare_exchange_ptrs(other_ptrs, my_ptrs))
            {
                my_ptrs = load_ptrs(memory_order_acquire);
                other_ptrs = other.load_ptrs(memory_order_acquire);
            }
        }

        void reset(T *p = nullptr) noexcept
        {
            PointerPair new_ptrs{nullptr, nullptr};
            if (p) {
                new_ptrs = PointerPair{new BasicControlBlock<T>(p), p};
            }
            auto old_ptrs = exchange_ptrs(new_ptrs);
            if (old_ptrs.cb)
            {
                release(old_ptrs);
            }
        }

        ~LockFreeSharedWithWeakPtr()
        {
            auto old_ptrs = load_ptrs(memory_order_acquire);
            if (old_ptrs.cb)
            {
                release(old_ptrs);
            }
        }

        T &operator*() const noexcept { return *get(); }
        T *operator->() const noexcept { return get(); }
        T *get() const noexcept { return load_ptrs(memory_order_acquire).ptr; }

        explicit operator bool() const noexcept
        {
            return load_ptrs(memory_order_acquire).ptr != nullptr;
        }

        int use_count() const noexcept
        {
            auto current = load_ptrs(memory_order_acquire);
            return current.cb ? current.cb->use_count_val() : 0;
        }

        int weak_count() const noexcept
        {
            auto current = load_ptrs(memory_order_acquire);
            return current.cb ? current.cb->weak_count_val() : 0;
        }

    private:
        PointerPair exchange_ptrs(const PointerPair &new_ptrs) noexcept
        {
            if constexpr (has_native_dwcas())
            {
                return ptrs.exchange(new_ptrs, memory_order_acq_rel);
            }
            else
            {
                auto old = load_ptrs(memory_order_acquire);
                try_update_pointers(new_ptrs.cb, new_ptrs.ptr);
                return old;
            }
        }

        void release(const PointerPair &old_ptrs) noexcept
        {
            if (old_ptrs.cb && old_ptrs.cb->removeRef() == 1)
            {
                atomic_thread_fence(memory_order_acquire);
                old_ptrs.cb->destroy_object();

                if (old_ptrs.cb->weak_count_val() == 0)
                {
                    old_ptrs.cb->destroy_this();
                }
            }
        }
    };

    // make_shared implementation
    template<typename T, typename... Args>
    LockFreeSharedWithWeakPtr<T> make_shared_custom(Args&&... args) {
        using AllocType = std::allocator<T>;
        AllocType alloc;
        using CB = ControlBlockMakeShared<T, AllocType>;
        using CBAlloc = typename std::allocator_traits<AllocType>::template rebind_alloc<CB>;
        
        CBAlloc cb_alloc(alloc);
        CB* cb = cb_alloc.allocate(1);
        
        try {
            new (cb) CB(alloc, std::forward<Args>(args)...);
            return LockFreeSharedWithWeakPtr<T>(cb->get_ptr(), 
                [cb](T*) { cb->destroy_this(); }, alloc);
        } catch (...) {
            cb_alloc.deallocate(cb, 1);
            throw;
        }
    }

    // Utility functions for array support
    template<typename T>
    LockFreeSharedWithWeakPtr<T> make_shared_array(size_t size) {
        auto* cb = new ControlBlockArray<T>(size);
        return LockFreeSharedWithWeakPtr<T>(cb->get(), 
            [cb](T*) { cb->destroy_this(); });
    }

    // Enhanced make_shared with strong exception guarantee
    template<typename T, typename... Args>
    LockFreeSharedWithWeakPtr<T> make_shared_safe(Args&&... args) {
        using AllocType = std::allocator<T>;
        AllocType alloc;
        using MakeSharedCB = ControlBlockMakeShared<T, AllocType>;
        using MakeSharedCBAlloc = typename std::allocator_traits<AllocType>::
            template rebind_alloc<MakeSharedCB>;

        struct Guard {
            MakeSharedCB* cb;
            MakeSharedCBAlloc& alloc;
            bool committed = false;
            
            Guard(MakeSharedCB* p, MakeSharedCBAlloc& a) : cb(p), alloc(a) {}
            ~Guard() {
                if (!committed && cb) {
                    alloc.deallocate(cb, 1);
                }
            }
        };

        MakeSharedCBAlloc cb_alloc(alloc);
        auto* cb = cb_alloc.allocate(1);
        
        Guard guard(cb, cb_alloc);
        new (cb) MakeSharedCB(alloc, std::forward<Args>(args)...);
        auto ptr = LockFreeSharedWithWeakPtr<T>(cb->get_ptr(), 
            [cb](T*) { cb->destroy_this(); }, alloc);
        guard.committed = true;
        return ptr;
    }

    // Atomic operations optimization
    template<typename T>
    void atomic_store(LockFreeSharedWithWeakPtr<T>& ptr, 
                     LockFreeSharedWithWeakPtr<T> desired) noexcept {
        ptr = move(desired);
    }

    template<typename T>
    LockFreeSharedWithWeakPtr<T> atomic_load(const LockFreeSharedWithWeakPtr<T>& ptr) noexcept {
        return ptr;
    }

    template<typename T>
    LockFreeSharedWithWeakPtr<T> atomic_exchange(LockFreeSharedWithWeakPtr<T>& ptr,
                                                LockFreeSharedWithWeakPtr<T> desired) noexcept {
        LockFreeSharedWithWeakPtr<T> old;
        ptr.swap(old);
        ptr = move(desired);
        return old;
    }

    // Type traits support
    template<typename T>
    struct is_lock_free_shared_ptr : false_type {};

    template<typename T>
    struct is_lock_free_shared_ptr<LockFreeSharedWithWeakPtr<T>> : true_type {};

    template<typename T>
    inline constexpr bool is_lock_free_shared_ptr_v = 
        is_lock_free_shared_ptr<T>::value;

    // Comparison operators
    template<typename T, typename U>
    bool operator==(const LockFreeSharedWithWeakPtr<T>& lhs, 
                   const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        return lhs.get() == rhs.get();
    }

    template<typename T, typename U>
    bool operator!=(const LockFreeSharedWithWeakPtr<T>& lhs,
                   const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        return !(lhs == rhs);
    }

    template<typename T, typename U>
    bool operator<(const LockFreeSharedWithWeakPtr<T>& lhs,
                  const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        using P = typename common_type<T*, U*>::type;
        return less<P>()(lhs.get(), rhs.get());
    }

    template<typename T, typename U>
    bool operator<=(const LockFreeSharedWithWeakPtr<T>& lhs,
                   const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        return !(rhs < lhs);
    }

    template<typename T, typename U>
    bool operator>(const LockFreeSharedWithWeakPtr<T>& lhs,
                  const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        return rhs < lhs;
    }

    template<typename T, typename U>
    bool operator>=(const LockFreeSharedWithWeakPtr<T>& lhs,
                   const LockFreeSharedWithWeakPtr<U>& rhs) noexcept {
        return !(lhs < rhs);
    }

    template<typename T>
    bool operator==(const LockFreeSharedWithWeakPtr<T>& lhs, std::nullptr_t) noexcept {
        return !lhs;
    }

    template<typename T>
    bool operator==(std::nullptr_t, const LockFreeSharedWithWeakPtr<T>& rhs) noexcept {
        return !rhs;
    }

    template<typename T>
    bool operator!=(const LockFreeSharedWithWeakPtr<T>& lhs, std::nullptr_t) noexcept {
        return static_cast<bool>(lhs);
    }

    template<typename T>
    bool operator!=(std::nullptr_t, const LockFreeSharedWithWeakPtr<T>& rhs) noexcept {
        return static_cast<bool>(rhs);
    }

    template<typename T>
    class LockFreeWeakPtr {
    private:
        struct alignas(16) WeakPointerPair {
            ControlBlock* cb;
            T* ptr;
            bool operator==(const WeakPointerPair& other) const noexcept {
                return cb == other.cb && ptr == other.ptr;
            }
        };

        atomic<WeakPointerPair> ptrs;

    public:
        constexpr LockFreeWeakPtr() noexcept = default;

        LockFreeWeakPtr(const LockFreeSharedWithWeakPtr<T>& shared) noexcept {
            auto shared_ptrs = shared.load_ptrs(memory_order_acquire);
            if (shared_ptrs.cb) {
                shared_ptrs.cb->addWeakRef();
                ptrs.store(WeakPointerPair{shared_ptrs.cb, shared_ptrs.ptr}, 
                          memory_order_release);
            }
        }

        LockFreeWeakPtr(const LockFreeWeakPtr& other) noexcept {
            auto other_ptrs = other.ptrs.load(memory_order_acquire);
            if (other_ptrs.cb) {
                other_ptrs.cb->addWeakRef();
                ptrs.store(other_ptrs, memory_order_release);
            }
        }

        LockFreeWeakPtr& operator=(const LockFreeWeakPtr& other) noexcept {
            if (this != &other) {
                auto new_ptrs = other.ptrs.load(memory_order_acquire);
                if (new_ptrs.cb) {
                    new_ptrs.cb->addWeakRef();
                }

                auto old_ptrs = ptrs.exchange(new_ptrs, memory_order_acq_rel);
                if (old_ptrs.cb) {
                    old_ptrs.cb->removeWeakRef();
                }
            }
            return *this;
        }

        ~LockFreeWeakPtr() noexcept {
            auto old_ptrs = ptrs.load(memory_order_acquire);
            if (old_ptrs.cb) {
                old_ptrs.cb->removeWeakRef();
            }
        }

        LockFreeSharedWithWeakPtr<T> lock() const noexcept {
            while (true) {
                auto current = ptrs.load(memory_order_acquire);
                if (!current.cb) {
                    return LockFreeSharedWithWeakPtr<T>();
                }

                // Try to increment the use count
                current.cb->addRef();
                
                // Verify the control block hasn't changed
                auto after = ptrs.load(memory_order_acquire);
                if (current.cb == after.cb) {
                    return LockFreeSharedWithWeakPtr<T>(
                        current.ptr, 
                        [current](T*) { current.cb->destroy_this(); });
                }

                // Control block changed, undo increment and retry
                current.cb->removeRef();
            }
        }

        long use_count() const noexcept {
            auto current = ptrs.load(memory_order_acquire);
            return current.cb ? current.cb->use_count_val() : 0;
        }

        bool expired() const noexcept {
            auto current = ptrs.load(memory_order_acquire);
            return !current.cb || current.cb->use_count_val() == 0;
        }

        void reset() noexcept {
            auto old_ptrs = ptrs.exchange(WeakPointerPair{nullptr, nullptr}, 
                                        memory_order_acq_rel);
            if (old_ptrs.cb) {
                old_ptrs.cb->removeWeakRef();
            }
        }

        bool owner_before(const LockFreeWeakPtr& other) const noexcept {
            return ptrs.load(memory_order_acquire).cb < 
                   other.ptrs.load(memory_order_acquire).cb;
        }

        bool owner_before(const LockFreeSharedWithWeakPtr<T>& other) const noexcept {
            return ptrs.load(memory_order_acquire).cb < 
                   other.load_ptrs(memory_order_acquire).cb;
        }
    };
}

// Hash specialization in std namespace
export namespace std {
    template<typename T>
    struct hash<ThreadSafeWorld::LockFreeSharedWithWeakPtr<T>> {
        size_t operator()(const ThreadSafeWorld::LockFreeSharedWithWeakPtr<T>& ptr) const noexcept {
            return hash<T*>()(ptr.get());
        }
    };
}