#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <utility> // For std::move, std::swap
#include <chrono>  // For sleep
#include <atomic>  // For std::atomic in CallsTracker
#include <cstddef> // For std::nullptr_t in CallsTracker

// // Assuming your module is correctly set up in your build system (CMake, etc.)
import leonrahul.LockFreeSharedPtr; // Or LockFreeSharedPtrGemini if testing that one

using namespace std;
using namespace std::chrono_literals; // Enables the 1ns, 1ms, etc. literals

// --- Add CallsTracker Definition HERE ---
struct CallsTracker
{
    static std::atomic<int> constructor_calls;
    static std::atomic<int> destructor_calls;
    int id; // Optional: identify specific instances

    CallsTracker(int i = 0) : id(i)
    {
        constructor_calls.fetch_add(1, memory_order_release);
    }

    virtual ~CallsTracker()
    {
        destructor_calls.fetch_add(1, memory_order_release);
    }

    // Prevent copying/moving to simplify lifetime tracking
    CallsTracker(const CallsTracker &) = delete;
    CallsTracker &operator=(const CallsTracker &) = delete;
    CallsTracker(CallsTracker &&) = delete;
    CallsTracker &operator=(CallsTracker &&) = delete;

    static void reset()
    {
        constructor_calls = 0;
        destructor_calls = 0;
    }
};

// Define the static members outside the class definition
std::atomic<int> CallsTracker::constructor_calls = 0;
std::atomic<int> CallsTracker::destructor_calls = 0;
// --- End of CallsTracker Definition ---


class LockFreeSharedPtrTest : public ::testing::Test
{

    protected: 
    void SetUp() override
    {
        CallsTracker::reset();
        testPtr_.reset(new CallsTracker(1));
        ASSERT_EQ(CallsTracker::constructor_calls, 1);
        ASSERT_EQ(CallsTracker::destructor_calls, 0); 
    }

    void TearDown() override
    {

    }

    ThreadSafeWorld::LockFreeSharedPtr<CallsTracker>  getSharedPtr()
    {
        return testPtr_;
    }

    ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> testPtr_;
};




// // --- Basic Construction and Destruction ---


TEST_F(LockFreeSharedPtrTest, DefaultConstructor)
{
    ThreadSafeWorld::LockFreeSharedPtr < CallsTracker> aPtr(nullptr);
    EXPECT_EQ(aPtr.get(), nullptr );
    
}

TEST_F(LockFreeSharedPtrTest, MultipleThreadAccess)
{
    atomic<bool> start_flag{false}; 
    int tCount = 10; 
     // at destruction of vector and so jthread , join will be called 
     {
        vector<jthread> threads;

        ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> localPtr =   getSharedPtr(); 

        for (int i = 0; i < tCount; ++i)
        {
            threads.emplace_back([&start_flag, &localPtr]
                                 {

                //create thousnad copies and assignemnt 
                while(!start_flag.load(memory_order_acquire)) //busy wait 
                {
                    std::cout << "Yielding thread..." << std::endl;
                    this_thread::yield(); 
                } //Ensures all thread starts almost at the same time
                for (int j =0; j <1000 ; ++j)
                {
                    ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> somePtr(localPtr); // Copy COnstructor
                    ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> somePtrB{new CallsTracker(1)};

                    somePtrB = somePtr;  //assignment operator
                } });

                if(i == 9)
                {
                    start_flag.store(true, memory_order_release);
                }
            
            
        }
    }

    // for (auto &t : threads)
    // {
    //     if (t.joinable())
    //         t.join();
    // }

    //EXPECT_EQ(localPtr.getCount(), 2);

    EXPECT_EQ(CallsTracker::constructor_calls.load(memory_order_acquire), 10001);
    EXPECT_EQ(CallsTracker::destructor_calls.load(memory_order_acquire), 10000);
    EXPECT_EQ(testPtr_.getCount(), 1); 
}

// TEST_F(LockFreeSharedPtrTest, DefaultConstructor)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr;
//     ASSERT_EQ(getPtr(ptr), nullptr);
//     ASSERT_EQ(getCb(ptr), nullptr);
//     ASSERT_EQ(CallsTracker::constructor_calls, 0);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
// } // Test destructor of empty pointer

// TEST_F(LockFreeSharedPtrTest, NullptrConstructor)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr(nullptr);
//     ASSERT_EQ(getPtr(ptr), nullptr);
//     ASSERT_EQ(getCb(ptr), nullptr);
//     ASSERT_EQ(CallsTracker::constructor_calls, 0);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
// } // Test destructor of empty pointer

// TEST_F(LockFreeSharedPtrTest, RawPointerConstructorLifecycle)
// {
//     CallsTracker *raw_ptr_val = nullptr;
//     {
//         ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr(new CallsTracker(1));
//         raw_ptr_val = getPtr(ptr); // Get the raw pointer value
//         ASSERT_NE(raw_ptr_val, nullptr);
//         ASSERT_NE(getCb(ptr), nullptr);
//         ASSERT_EQ(CallsTracker::constructor_calls, 1);
//         ASSERT_EQ(CallsTracker::destructor_calls, 0);
//         // Need use_count() to verify count == 1
//     } // ptr goes out of scope here
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Should be deleted
// }

// TEST_F(LockFreeSharedPtrTest, RawPointerConstructorNull)
// {
//     {
//         // Note: Your constructor `LockFreeSharedPtr(T *ptr)` currently allocates
//         // a ControlBlock even if ptr is null. This is likely not intended.
//         // Assuming it should NOT allocate if ptr is null:
//         // ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr(nullptr);
//         // ASSERT_EQ(getPtr(ptr), nullptr);
//         // ASSERT_EQ(getCb(ptr), nullptr);

//         // Based on current implementation (allocates CB even for null):
//         ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr(static_cast<CallsTracker *>(nullptr));
//         ASSERT_EQ(getPtr(ptr), nullptr);
//         ASSERT_NE(getCb(ptr), nullptr); // CB is allocated
//         ASSERT_EQ(CallsTracker::constructor_calls, 0);
//         ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     } // CB should be deleted here (but not ptr, as it was null)
//     ASSERT_EQ(CallsTracker::constructor_calls, 0);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // This test highlights a potential issue in the raw pointer constructor.
//     // Consider changing it to:
//     // LockFreeSharedPtr(T *p) : ptr{p}, cb{p ? new ControlBlock() : nullptr} {}
// }

// // --- Copy Operations ---

// TEST_F(LockFreeSharedPtrTest, CopyConstructorLifecycle)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(2));
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_NE(getCb(ptr1), nullptr);
//     CallsTracker *raw_ptr_val = getPtr(ptr1);
//     ThreadSafeWorld::ControlBlock *cb_val = getCb(ptr1);

//     {
//         ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2 = ptr1; // Copy construct
//         ASSERT_EQ(getPtr(ptr2), raw_ptr_val);
//         ASSERT_EQ(getCb(ptr2), cb_val); // Should share the control block
//         ASSERT_EQ(CallsTracker::constructor_calls, 1);
//         ASSERT_EQ(CallsTracker::destructor_calls, 0);
//         // Need use_count() == 2
//     } // ptr2 goes out of scope
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0); // ptr1 still holds it
//     // Need use_count() == 1

//     // Reset ptr1 explicitly to test final destruction
//     ptr1 = nullptr; // Assigning nullptr should release the resource
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1);
// }

// TEST_F(LockFreeSharedPtrTest, CopyConstructorEmpty)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1;
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2 = ptr1; // Copy empty
//     ASSERT_EQ(getPtr(ptr1), nullptr);
//     ASSERT_EQ(getCb(ptr1), nullptr);
//     ASSERT_EQ(getPtr(ptr2), nullptr);
//     ASSERT_EQ(getCb(ptr2), nullptr);
//     ASSERT_EQ(CallsTracker::constructor_calls, 0);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
// }

// TEST_F(LockFreeSharedPtrTest, CopyAssignmentLifecycle)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1;
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2(new CallsTracker(3));
//     CallsTracker *initial_ptr2_val = getPtr(ptr2);
//     ThreadSafeWorld::ControlBlock *initial_cb2_val = getCb(ptr2);

//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);

//     ptr1 = ptr2; // Copy assign ptr2 to ptr1

//     ASSERT_EQ(getPtr(ptr1), initial_ptr2_val);
//     ASSERT_EQ(getCb(ptr1), initial_cb2_val);
//     ASSERT_EQ(getPtr(ptr2), initial_ptr2_val); // ptr2 should be unchanged
//     ASSERT_EQ(getCb(ptr2), initial_cb2_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // Need use_count() == 2

//     // Assign something else to ptr2 to test ptr1 lifetime
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr3(new CallsTracker(4));
//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ptr2 = ptr3; // Assign ptr3 to ptr2. Old ptr2 ref count decreases.

//     ASSERT_EQ(getPtr(ptr1), initial_ptr2_val); // ptr1 still holds original object
//     ASSERT_EQ(getCb(ptr1), initial_cb2_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0); // Original object not deleted yet
//     // Need use_count() == 1 for ptr1's object

//     ptr1 = nullptr; // Reset ptr1, should delete original object
//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Original object (3) deleted

//     ptr2 = nullptr;                                    // Reset ptr2
//     ASSERT_EQ(CallsTracker::destructor_calls, 2); // Second object (4) deleted
//     ptr3 = nullptr;                                    // Reset ptr3
//     ASSERT_EQ(CallsTracker::destructor_calls, 2); // Count doesn't change (was already held by ptr2)
// }

// TEST_F(LockFreeSharedPtrTest, CopyAssignmentSelf)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(5));
//     CallsTracker *initial_ptr1_val = getPtr(ptr1);
//     ThreadSafeWorld::ControlBlock *initial_cb1_val = getCb(ptr1);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);

//     ptr1 = ptr1; // Self-assignment

//     ASSERT_EQ(getPtr(ptr1), initial_ptr1_val); // Should be unchanged
//     ASSERT_EQ(getCb(ptr1), initial_cb1_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // Need use_count() == 1

//     ptr1 = nullptr; // Reset ptr1
//     ASSERT_EQ(CallsTracker::destructor_calls, 1);
// }

// TEST_F(LockFreeSharedPtrTest, CopyAssignmentToExisting)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(6));
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2(new CallsTracker(7));
//     CallsTracker *initial_ptr2_val = getPtr(ptr2);
//     ThreadSafeWorld::ControlBlock *initial_cb2_val = getCb(ptr2);

//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);

//     ptr1 = ptr2; // Assign ptr2 to ptr1 (which already holds an object)

//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // ptr1's original object (6) should be destroyed
//     ASSERT_EQ(getPtr(ptr1), initial_ptr2_val);         // ptr1 now points to ptr2's object
//     ASSERT_EQ(getCb(ptr1), initial_cb2_val);
//     ASSERT_EQ(getPtr(ptr2), initial_ptr2_val); // ptr2 is unchanged
//     ASSERT_EQ(getCb(ptr2), initial_cb2_val);
//     // Need use_count() == 2 for object 7

//     ptr1 = nullptr;
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Count doesn't change yet
//     ptr2 = nullptr;
//     ASSERT_EQ(CallsTracker::destructor_calls, 2); // Object 7 destroyed
// }

// // --- Move Operations ---

// TEST_F(LockFreeSharedPtrTest, MoveConstructorLifecycle)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(8));
//     CallsTracker *initial_ptr1_val = getPtr(ptr1);
//     ThreadSafeWorld::ControlBlock *initial_cb1_val = getCb(ptr1);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);

//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2 = std::move(ptr1); // Move construct

//     // Verify ptr2 took ownership
//     ASSERT_EQ(getPtr(ptr2), initial_ptr1_val);
//     ASSERT_EQ(getCb(ptr2), initial_cb1_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // Need use_count() == 1 for ptr2

//     // Verify ptr1 is empty
//     ASSERT_EQ(getPtr(ptr1), nullptr);
//     ASSERT_EQ(getCb(ptr1), nullptr);

//     ptr2 = nullptr;                                    // Reset ptr2
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Now destroyed
// }

// TEST_F(LockFreeSharedPtrTest, MoveConstructorEmpty)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1;
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2 = std::move(ptr1); // Move empty
//     ASSERT_EQ(getPtr(ptr1), nullptr);
//     ASSERT_EQ(getCb(ptr1), nullptr);
//     ASSERT_EQ(getPtr(ptr2), nullptr);
//     ASSERT_EQ(getCb(ptr2), nullptr);
//     ASSERT_EQ(CallsTracker::constructor_calls, 0);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
// }

// TEST_F(LockFreeSharedPtrTest, MoveAssignmentLifecycle)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(9));
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2(new CallsTracker(10));
//     CallsTracker *initial_ptr1_val = getPtr(ptr1); // To check deletion
//     CallsTracker *initial_ptr2_val = getPtr(ptr2);
//     ThreadSafeWorld::ControlBlock *initial_cb2_val = getCb(ptr2);

//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);

//     ptr1 = std::move(ptr2); // Move assign ptr2 to ptr1

//     // Verify ptr1 took ownership of ptr2's object
//     ASSERT_EQ(getPtr(ptr1), initial_ptr2_val);
//     ASSERT_EQ(getCb(ptr1), initial_cb2_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // ptr1's original object (9) should be destroyed
//     // Need use_count() == 1 for ptr1 (object 10)

//     // Verify ptr2 is empty
//     ASSERT_EQ(getPtr(ptr2), nullptr);
//     ASSERT_EQ(getCb(ptr2), nullptr);

//     ptr1 = nullptr;                                    // Reset ptr1
//     ASSERT_EQ(CallsTracker::destructor_calls, 2); // ptr2's original object (10) should be destroyed
// }

// TEST_F(LockFreeSharedPtrTest, MoveAssignmentSelf)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(11));
//     CallsTracker *initial_ptr1_val = getPtr(ptr1);
//     ThreadSafeWorld::ControlBlock *initial_cb1_val = getCb(ptr1);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);

//     ptr1 = std::move(ptr1); // Self move-assignment

//     // Your implementation correctly handles self-move via copy-and-swap idiom
//     // The temporary created inside the assignment operator takes ownership,
//     // then swaps back. The net effect is no change.
//     ASSERT_NE(getPtr(ptr1), nullptr);          // Should not be null
//     ASSERT_EQ(getPtr(ptr1), initial_ptr1_val); // Should be unchanged
//     ASSERT_EQ(getCb(ptr1), initial_cb1_val);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // Need use_count() == 1

//     ptr1 = nullptr; // Reset ptr1
//     ASSERT_EQ(CallsTracker::destructor_calls, 1);
// }

// TEST_F(LockFreeSharedPtrTest, MoveAssignmentEmpty)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(12));
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2; // Empty

//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);

//     ptr1 = std::move(ptr2); // Move assign empty ptr2 to ptr1

//     ASSERT_EQ(getPtr(ptr1), nullptr); // ptr1 should become empty
//     ASSERT_EQ(getCb(ptr1), nullptr);
//     ASSERT_EQ(getPtr(ptr2), nullptr); // ptr2 remains empty
//     ASSERT_EQ(getCb(ptr2), nullptr);
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // ptr1's original object (12) destroyed
// }

// // --- Swap ---

// TEST_F(LockFreeSharedPtrTest, SwapMember)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr1(new CallsTracker(13));
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> ptr2(new CallsTracker(14));

//     CallsTracker *initial_ptr1_val = getPtr(ptr1);
//     ThreadSafeWorld::ControlBlock *initial_cb1_val = getCb(ptr1);
//     CallsTracker *initial_ptr2_val = getPtr(ptr2);
//     ThreadSafeWorld::ControlBlock *initial_cb2_val = getCb(ptr2);

//     ASSERT_EQ(CallsTracker::constructor_calls, 2);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);

//     ptr1.swap(ptr2);

//     ASSERT_EQ(getPtr(ptr1), initial_ptr2_val);
//     ASSERT_EQ(getCb(ptr1), initial_cb2_val);
//     ASSERT_EQ(getPtr(ptr2), initial_ptr1_val);
//     ASSERT_EQ(getCb(ptr2), initial_cb1_val);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0); // No objects destroyed by swap

//     ptr1 = nullptr;
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Object 14 destroyed
//     ptr2 = nullptr;
//     ASSERT_EQ(CallsTracker::destructor_calls, 2); // Object 13 destroyed
// }

// // --- Thread Safety (Basic) ---

// TEST_F(LockFreeSharedPtrTest, ConcurrentCopiesAndDestruction)
// {
//     ThreadSafeWorld::LockFreeSharedPtr<CallsTracker> shared_ptr(new CallsTracker(15));
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);

//     const int num_threads = 50; // Increase threads for more contention
//     const int copies_per_thread = 10;
//     std::vector<std::thread> threads;
//     std::atomic<bool> start_flag = false;

//     for (int i = 0; i < num_threads; ++i)
//     {
//         threads.emplace_back([&shared_ptr, &start_flag, copies_per_thread] {
//             // Spin until start flag is set to increase concurrency
//             while (!start_flag.load(std::memory_order_acquire))
//             {
//                 std::this_thread::yield();
//             }

//             std::vector<ThreadSafeWorld::LockFreeSharedPtr<CallsTracker>> local_copies;
//             for (int j = 0; j < copies_per_thread; ++j)
//             {
//                 local_copies.push_back(shared_ptr); // Create copies
//             }
//             // Copies go out of scope here, decrementing count concurrently
//         });
//     }

//     // Signal threads to start roughly together
//     start_flag.store(true, std::memory_order_release);

//     // Wait for all threads to finish
//     for (auto &t : threads)
//     {
//         t.join();
//     }

//     // At this point, only the original shared_ptr should hold a reference
//     ASSERT_EQ(CallsTracker::constructor_calls, 1);
//     ASSERT_EQ(CallsTracker::destructor_calls, 0);
//     // Need use_count() == 1

//     shared_ptr = nullptr;                              // Release the last reference
//     ASSERT_EQ(CallsTracker::destructor_calls, 1); // Should be destroyed exactly once
// }

// // Add main function if not linking with gtest_main
// // int main(int argc, char **argv) {
// //     ::testing::InitGoogleTest(&argc, argv);
// //     return RUN_ALL_TESTS();
// // }
