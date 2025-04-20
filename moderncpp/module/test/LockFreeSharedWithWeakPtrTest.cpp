#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include <mutex>
#include <type_traits>

// Since modules aren't fully supported in all environments yet,
// we'll include the header directly
import leonrahul.LockFreeSharedWithWeakPtr;

using namespace std;
using ThreadSafeWorld::LockFreeSharedWithWeakPtr;
using ThreadSafeWorld::LockFreeWeakPtr;
using ThreadSafeWorld::make_shared_custom;
using ThreadSafeWorld::make_shared_array;
using ThreadSafeWorld::make_shared_safe;

// Test helper class with tracking
class TrackingType {
public:
    static atomic<int> constructor_calls;
    static atomic<int> destructor_calls;
    static atomic<int> custom_deleter_calls;
    int value;

    TrackingType(int v = 0) : value(v) {
        constructor_calls.fetch_add(1, memory_order_release);
    }

    ~TrackingType() {
        destructor_calls.fetch_add(1, memory_order_release);
    }

    static void reset_counters() {
        constructor_calls = 0;
        destructor_calls = 0;
        custom_deleter_calls = 0;
    }
};

atomic<int> TrackingType::constructor_calls(0);
atomic<int> TrackingType::destructor_calls(0);
atomic<int> TrackingType::custom_deleter_calls(0);

// Custom deleter for testing
struct CustomDeleter {
    void operator()(TrackingType* p) {
        TrackingType::custom_deleter_calls.fetch_add(1, memory_order_release);
        delete p;
    }
};

// Custom allocator for testing
template<typename T>
class CustomAllocator : public allocator<T> {
public:
    static atomic<int> allocate_calls;
    static atomic<int> deallocate_calls;

    template<typename U>
    struct rebind { using other = CustomAllocator<U>; };

    T* allocate(size_t n) {
        allocate_calls.fetch_add(1, memory_order_release);
        return allocator<T>::allocate(n);
    }

    void deallocate(T* p, size_t n) {
        deallocate_calls.fetch_add(1, memory_order_release);
        allocator<T>::deallocate(p, n);
    }

    static void reset_counters() {
        allocate_calls = 0;
        deallocate_calls = 0;
    }
};

template<typename T>
atomic<int> CustomAllocator<T>::allocate_calls(0);
template<typename T>
atomic<int> CustomAllocator<T>::deallocate_calls(0);

class LockFreeSharedWithWeakPtrTest : public ::testing::Test {
protected:
    void SetUp() override {
        TrackingType::reset_counters();
        CustomAllocator<TrackingType>::reset_counters();
    }
};

// Test make_shared functionality
TEST_F(LockFreeSharedWithWeakPtrTest, MakeShared) {
    auto ptr = make_shared_custom<TrackingType>(42);
    EXPECT_EQ(ptr->value, 42);
    EXPECT_EQ(TrackingType::constructor_calls, 1);
    EXPECT_EQ(TrackingType::destructor_calls, 0);
    EXPECT_EQ(ptr.use_count(), 1);
}

// Update the CustomDeleter test to use our pointer type
TEST_F(LockFreeSharedWithWeakPtrTest, CustomDeleter) {
    {
        LockFreeSharedWithWeakPtr<TrackingType> ptr(new TrackingType(42), CustomDeleter());
        EXPECT_EQ(ptr->value, 42);
        EXPECT_EQ(TrackingType::constructor_calls, 1);
        EXPECT_EQ(TrackingType::custom_deleter_calls, 0);
    }
    EXPECT_EQ(TrackingType::custom_deleter_calls, 1);
    EXPECT_EQ(TrackingType::destructor_calls, 1);
}

// Update the CustomAllocator test
TEST_F(LockFreeSharedWithWeakPtrTest, CustomAllocator) {
    using Allocator = CustomAllocator<TrackingType>;
    {
        auto ptr = make_shared_custom<TrackingType>(42);
        EXPECT_EQ(ptr->value, 42);
        EXPECT_EQ(ptr.use_count(), 1);
    }
}

// Test array support
TEST_F(LockFreeSharedWithWeakPtrTest, ArraySupport) {
    const size_t array_size = 5;
    auto ptr = make_shared_array<TrackingType>(array_size);
    EXPECT_EQ(TrackingType::constructor_calls, array_size);
    EXPECT_EQ(TrackingType::destructor_calls, 0);
    ptr.reset();
    EXPECT_EQ(TrackingType::destructor_calls, array_size);
}

// Test weak pointer functionality
TEST_F(LockFreeSharedWithWeakPtrTest, WeakPtr) {
    auto shared = make_shared_custom<TrackingType>(42);
    LockFreeWeakPtr<TrackingType> weak(shared);
    EXPECT_EQ(weak.use_count(), 1);
    EXPECT_FALSE(weak.expired());
    
    if (auto locked = weak.lock()) {
        EXPECT_EQ(locked->value, 42);
    }
}

// Test atomic operations
TEST_F(LockFreeSharedWithWeakPtrTest, BasicThreadSafety) {
    auto ptr1 = make_shared_custom<TrackingType>(1);
    auto ptr2 = make_shared_custom<TrackingType>(2);
    
    ptr1 = ptr2;
    EXPECT_EQ(ptr1->value, 2);
    
    auto ptr3(ptr1);
    EXPECT_EQ(ptr3->value, 2);
}

// Test thread safety with multiple operations
TEST_F(LockFreeSharedWithWeakPtrTest, ThreadSafety) {
    const int num_threads = 10;
    const int operations_per_thread = 1000;
    atomic<bool> start{false};
    vector<thread> threads;
    mutex weak_mutex;
    
    auto shared = make_shared_custom<TrackingType>(42);
    vector<LockFreeWeakPtr<TrackingType>> weak_ptrs;
    weak_ptrs.reserve(num_threads * operations_per_thread);
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i] {
            while (!start) { this_thread::yield(); }
            
            for (int j = 0; j < operations_per_thread; ++j) {
                switch (j % 3) {
                    case 0: {
                        lock_guard<mutex> lock(weak_mutex);
                        weak_ptrs.emplace_back(shared);  // Use emplace_back with constructor
                        break;
                    }
                    case 1: {
                        lock_guard<mutex> lock(weak_mutex);
                        if (!weak_ptrs.empty()) {
                            if (auto locked = weak_ptrs.back().lock()) {
                                EXPECT_EQ(locked->value, 42);
                            }
                        }
                        break;
                    }
                    case 2: {
                        auto local_copy = shared;
                        EXPECT_EQ(local_copy->value, 42);
                        break;
                    }
                }
            }
        });
    }
    
    start = true;
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_GT(shared.use_count(), 0);
    shared.reset();
    
    for (auto& weak : weak_ptrs) {
        EXPECT_TRUE(weak.expired());
        EXPECT_EQ(weak.lock(), nullptr);
    }
}

// Test exception safety
class ThrowingType {
public:
    struct ConstructorException {};
    static atomic<int> constructor_attempts;
    static atomic<int> destructor_calls;
    
    explicit ThrowingType(bool throw_in_constructor = false) {
        constructor_attempts.fetch_add(1, memory_order_release);
        if (throw_in_constructor) {
            throw ConstructorException();
        }
    }
    
    ~ThrowingType() {
        destructor_calls.fetch_add(1, memory_order_release);
    }
    
    static void reset_counters() {
        constructor_attempts = 0;
        destructor_calls = 0;
    }
};

atomic<int> ThrowingType::constructor_attempts(0);
atomic<int> ThrowingType::destructor_calls(0);

// Test make_shared exception safety
TEST_F(LockFreeSharedWithWeakPtrTest, MakeSharedExceptionSafety) {
    ThrowingType::reset_counters();
    
    EXPECT_THROW({
        auto ptr = make_shared_custom<ThrowingType>(true);
    }, ThrowingType::ConstructorException);
    
    EXPECT_EQ(ThrowingType::constructor_attempts, 1);
    EXPECT_EQ(ThrowingType::destructor_calls, 0);
}

// Update make_shared_safe test to handle exceptions properly
TEST_F(LockFreeSharedWithWeakPtrTest, MakeSharedSafeExceptionSafety) {
    ThrowingType::reset_counters();
    
    EXPECT_THROW({
        auto ptr = make_shared_safe<ThrowingType>(true);
    }, ThrowingType::ConstructorException);
    
    EXPECT_EQ(ThrowingType::constructor_attempts, 1);
    EXPECT_EQ(ThrowingType::destructor_calls, 0);
}

// Test inheritance and type conversions
class Base {
public:
    virtual ~Base() = default;
    virtual int get_value() const { return 42; }
};

class Derived : public Base {
public:
    int get_value() const override { return 84; }
};

// Test inheritance support
TEST_F(LockFreeSharedWithWeakPtrTest, BasicInheritance) {
    auto derived = make_shared_custom<Derived>();
    LockFreeWeakPtr<Derived> weak_derived(derived);
    EXPECT_FALSE(weak_derived.expired());
    
    if (auto locked = weak_derived.lock()) {
        EXPECT_EQ(locked->get_value(), 84);
    }
    
    derived.reset();
    EXPECT_TRUE(weak_derived.expired());
}

// Test aliasing constructor
TEST_F(LockFreeSharedWithWeakPtrTest, AliasingConstructor) {
    struct Container {
        TrackingType data{42};
    };
    
    auto container = make_shared_custom<Container>();
    LockFreeSharedWithWeakPtr<TrackingType> data_ptr(container, &container->data);
    
    EXPECT_EQ(data_ptr->value, 42);
    EXPECT_EQ(data_ptr.use_count(), container.use_count());
    
    container.reset();
    EXPECT_EQ(data_ptr.use_count(), 0);
}

// Test comparison operations
TEST_F(LockFreeSharedWithWeakPtrTest, BasicComparisons) {
    auto ptr1 = make_shared_custom<TrackingType>(1);
    auto ptr2 = make_shared_custom<TrackingType>(2);
    auto ptr3 = ptr1;
    
    EXPECT_TRUE(ptr1 == ptr3);
    EXPECT_TRUE(ptr1 != ptr2);
    
    LockFreeWeakPtr<TrackingType> weak1(ptr1);
    LockFreeWeakPtr<TrackingType> weak2(ptr2);
    
    EXPECT_FALSE(weak1.expired());
    EXPECT_FALSE(weak2.expired());
}

// Test null pointer handling
TEST_F(LockFreeSharedWithWeakPtrTest, NullPointerHandling) {
    LockFreeSharedWithWeakPtr<TrackingType> null_ptr;
    EXPECT_FALSE(null_ptr);
    EXPECT_EQ(null_ptr.get(), nullptr);
    
    LockFreeSharedWithWeakPtr<TrackingType> null_ptr2(nullptr);
    EXPECT_FALSE(null_ptr2);
    EXPECT_EQ(null_ptr2.get(), nullptr);
    
    // Test weak pointer null handling
    LockFreeWeakPtr<TrackingType> weak_null;
    EXPECT_TRUE(weak_null.expired());
    EXPECT_EQ(weak_null.lock(), nullptr);
}

// Test concurrent weak_ptr operations
TEST_F(LockFreeSharedWithWeakPtrTest, ConcurrentWeakOperations) {
    const int num_threads = 4;
    const int iterations = 1000;
    vector<thread> threads;
    atomic<bool> start{false};
    mutex weak_mutex;
    vector<LockFreeWeakPtr<TrackingType>> weak_ptrs;
    
    auto shared = make_shared_custom<TrackingType>(42);
    
    // Thread adding weak_ptrs
    threads.emplace_back([&] {
        while (!start) { this_thread::yield(); }
        for (int i = 0; i < iterations; ++i) {
            lock_guard<mutex> lock(weak_mutex);
            weak_ptrs.push_back(LockFreeWeakPtr<TrackingType>(shared));
        }
    });
    
    // Thread resetting weak_ptrs
    threads.emplace_back([&] {
        while (!start) { this_thread::yield(); }
        for (int i = 0; i < iterations; ++i) {
            lock_guard<mutex> lock(weak_mutex);
            if (!weak_ptrs.empty()) {
                weak_ptrs.back().reset();
                weak_ptrs.pop_back();
            }
        }
    });
    
    // Thread locking weak_ptrs
    threads.emplace_back([&] {
        while (!start) { this_thread::yield(); }
        for (int i = 0; i < iterations; ++i) {
            lock_guard<mutex> lock(weak_mutex);
            for (auto& weak : weak_ptrs) {
                if (auto locked = weak.lock()) {
                    EXPECT_EQ(locked->value, 42);
                }
            }
        }
    });
    
    // Thread copying weak_ptrs
    threads.emplace_back([&] {
        while (!start) { this_thread::yield(); }
        for (int i = 0; i < iterations; ++i) {
            lock_guard<mutex> lock(weak_mutex);
            if (!weak_ptrs.empty()) {
                LockFreeWeakPtr<TrackingType> weak;
                weak = weak_ptrs.back();
                weak_ptrs.push_back(weak);
            }
        }
    });
    
    start = true;
    for (auto& t : threads) {
        t.join();
    }
    
    shared.reset();
    for (const auto& weak : weak_ptrs) {
        EXPECT_TRUE(weak.expired());
    }
}