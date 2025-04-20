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
        atomic<int> use_count{1};
        atomic<int> weak_count{0};

    public:
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

    template <typename T>
    class LockFreeSharedWithWeakPtr
    {
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
            PointerPair new_ptrs{p ? new ControlBlock() : nullptr, p};
            store_ptrs(new_ptrs, memory_order_release);
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
            PointerPair new_ptrs{p ? new ControlBlock() : nullptr, p};
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
                delete old_ptrs.ptr;

                if (old_ptrs.cb->weak_count_val() == 0)
                {
                    delete old_ptrs.cb;
                }
            }
        }
    };
}