#include "gtest/gtest.h"
#include <cstring> // For strcmp, strlen
#include <vector>
#include <utility> // For std::move

// Import the module containing the class under test
import leonrahul.CustomStringArray;

// Use the namespace for convenience
using namespace StringWorld;

// Helper function to create a sample C-style string array for testing
// Note: The caller is responsible for managing the memory of the outer array,
// but the inner strings are literals and managed by the compiler.
char **createSampleData(std::vector<const char *> &data)
{
    if (data.empty())
    {
        return nullptr;
    }
    // IMPORTANT: This creates an array of pointers to *string literals*.
    // The CustomStringArray constructor should copy these.
    char **arr = new char *[data.size()];
    for (size_t i = 0; i < data.size(); ++i)
    {
        // Cast away constness - safe here because CustomStringArray copies.
        arr[i] = const_cast<char *>(data[i]);
    }
    return arr;
}

// Test Fixture (Optional but can be useful for common setup/teardown)
class CustomStringArrayTest : public ::testing::Test
{
protected:
    // Per-test setup (if needed)
    void SetUp() override
    {
        // Code here will be called immediately before each test
    }

    // Per-test teardown (if needed)
    void TearDown() override
    {
        // Code here will be called immediately after each test
    }

    // You can add helper methods or member variables here
};

// --- Test Cases ---

TEST_F(CustomStringArrayTest, DefaultConstructor)
{
    CustomStringArray arr;
    EXPECT_EQ(arr.getSize(), 0);
    EXPECT_EQ(arr.get(0), nullptr); // Accessing out of bounds should be safe (return nullptr)
}

TEST_F(CustomStringArrayTest, ParameterizedConstructorBasic)
{
    std::vector<const char *> initialData = {"hello", "world", "test"};
    char **c_arr = createSampleData(initialData);
    int size = initialData.size();

    CustomStringArray arr(c_arr, size);

    ASSERT_EQ(arr.getSize(), size);
    for (int i = 0; i < size; ++i)
    {
        ASSERT_NE(arr.get(i), nullptr); // Should not be null
        ASSERT_STREQ(arr.get(i), initialData[i]);
        // Verify deep copy: pointers should be different
        ASSERT_NE(arr.get(i), c_arr[i]);
    }

    delete[] c_arr; // Clean up the temporary array structure
}

TEST_F(CustomStringArrayTest, ParameterizedConstructorWithNull)
{
    std::vector<const char *> initialData = {"first", nullptr, "third"};
    char **c_arr = createSampleData(initialData);
    int size = initialData.size();

    CustomStringArray arr(c_arr, size);

    ASSERT_EQ(arr.getSize(), size);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), initialData[0]);
    ASSERT_EQ(arr.get(1), nullptr); // Should correctly copy the nullptr
    ASSERT_NE(arr.get(2), nullptr);
    ASSERT_STREQ(arr.get(2), initialData[2]);

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, ParameterizedConstructorEmpty)
{
    std::vector<const char *> initialData = {};
    char **c_arr = createSampleData(initialData); // Will be nullptr
    int size = 0;

    CustomStringArray arr(c_arr, size); // Should handle nullptr input gracefully

    EXPECT_EQ(arr.getSize(), 0);
    EXPECT_EQ(arr.get(0), nullptr);

    // delete[] c_arr; // No need to delete nullptr
}

TEST_F(CustomStringArrayTest, CopyConstructor)
{
    std::vector<const char *> initialData = {"copy", "me"};
    char **c_arr = createSampleData(initialData);
    int size = initialData.size();
    CustomStringArray original(c_arr, size);

    CustomStringArray copy = original; // Invoke copy constructor

    // 1. Check size and content of the copy
    ASSERT_EQ(copy.getSize(), original.getSize());
    for (int i = 0; i < copy.getSize(); ++i)
    {
        ASSERT_NE(copy.get(i), nullptr);
        ASSERT_STREQ(copy.get(i), original.get(i));
        // 2. Verify deep copy (different pointers)
        ASSERT_NE(copy.get(i), original.get(i));
    }

    // 3. Modify original and check copy is unaffected
    // Note: We can't directly modify original.get(0) easily without adding a set method.
    // Instead, let's test by adding to the original (if add works)
    original.add("new");
    ASSERT_EQ(copy.getSize(), size); // Copy size should remain unchanged
    ASSERT_STREQ(copy.get(0), "copy");

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, CopyConstructorEmpty)
{
    CustomStringArray original;
    CustomStringArray copy = original;

    EXPECT_EQ(copy.getSize(), 0);
    EXPECT_EQ(copy.get(0), nullptr);
}

TEST_F(CustomStringArrayTest, CopyAssignmentOperator)
{
    std::vector<const char *> initialData1 = {"one", "two"};
    char **c_arr1 = createSampleData(initialData1);
    CustomStringArray arr1(c_arr1, initialData1.size());

    std::vector<const char *> initialData2 = {"alpha", "beta", "gamma"};
    char **c_arr2 = createSampleData(initialData2);
    CustomStringArray arr2(c_arr2, initialData2.size());

    // Pre-check sizes
    ASSERT_EQ(arr1.getSize(), 2);
    ASSERT_EQ(arr2.getSize(), 3);

    arr1 = arr2; // Invoke copy assignment

    // 1. Check size and content of arr1 (should match arr2)
    ASSERT_EQ(arr1.getSize(), arr2.getSize());
    ASSERT_EQ(arr1.getSize(), 3);
    for (int i = 0; i < arr1.getSize(); ++i)
    {
        ASSERT_NE(arr1.get(i), nullptr);
        ASSERT_STREQ(arr1.get(i), arr2.get(i));
        // 2. Verify deep copy (different pointers)
        ASSERT_NE(arr1.get(i), arr2.get(i));
    }

    // 3. Check that arr2 is unaffected
    ASSERT_EQ(arr2.getSize(), 3);
    ASSERT_STREQ(arr2.get(0), "alpha");

    delete[] c_arr1;
    delete[] c_arr2;
}

TEST_F(CustomStringArrayTest, CopyAssignmentToEmpty)
{
    CustomStringArray arr1;
    std::vector<const char *> initialData2 = {"assign", "to", "empty"};
    char **c_arr2 = createSampleData(initialData2);
    CustomStringArray arr2(c_arr2, initialData2.size());

    arr1 = arr2;

    ASSERT_EQ(arr1.getSize(), 3);
    ASSERT_STREQ(arr1.get(1), "to");
    ASSERT_NE(arr1.get(1), arr2.get(1)); // Deep copy check

    delete[] c_arr2;
}

TEST_F(CustomStringArrayTest, CopyAssignmentFromEmpty)
{
    std::vector<const char *> initialData1 = {"assign", "from", "empty"};
    char **c_arr1 = createSampleData(initialData1);
    CustomStringArray arr1(c_arr1, initialData1.size());
    CustomStringArray arr2; // Empty

    arr1 = arr2; // Assign empty to non-empty

    EXPECT_EQ(arr1.getSize(), 0);
    EXPECT_EQ(arr1.get(0), nullptr);

    delete[] c_arr1;
}

TEST_F(CustomStringArrayTest, CopyAssignmentSelf)
{
    std::vector<const char *> initialData = {"self", "assign"};
    char **c_arr = createSampleData(initialData);
    CustomStringArray arr(c_arr, initialData.size());
    
    // Store pointers before self-assignment
    const char *firstPtr = arr.get(0);
    const char *secondPtr = arr.get(1);

    // Self-assignment should be safe and not change anything
    const CustomStringArray& ref = arr;
    arr = ref; // Use reference to avoid direct self-assign warning

    ASSERT_EQ(arr.getSize(), 2);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), "self");
    ASSERT_STREQ(arr.get(1), "assign");
    
    // Verify pointers remain the same (since it's self-assignment, no change should occur)
    ASSERT_EQ(arr.get(0), firstPtr);
    ASSERT_EQ(arr.get(1), secondPtr);

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, MoveConstructor)
{
    std::vector<const char *> initialData = {"move", "me"};
    char **c_arr = createSampleData(initialData);
    CustomStringArray original(c_arr, initialData.size());
    char *originalPtr0 = original.get(0); // Get pointer before move
    int originalSize = original.getSize();

    CustomStringArray moved = std::move(original); // Invoke move constructor

    // 1. Check the new (moved-to) object
    ASSERT_EQ(moved.getSize(), originalSize);
    ASSERT_NE(moved.get(0), nullptr);
    ASSERT_STREQ(moved.get(0), "move");
    ASSERT_STREQ(moved.get(1), "me");
    ASSERT_EQ(moved.get(0), originalPtr0); // Pointer should be the same (ownership transferred)

    // 2. Check the old (moved-from) object
    EXPECT_EQ(original.getSize(), 0);
    EXPECT_EQ(original.get(0), nullptr); // Should be in a valid empty state

    delete[] c_arr; // Still need to delete the outer array structure
}

TEST_F(CustomStringArrayTest, MoveConstructorFromEmpty)
{
    CustomStringArray original;
    CustomStringArray moved = std::move(original);

    EXPECT_EQ(moved.getSize(), 0);
    EXPECT_EQ(moved.get(0), nullptr);
    EXPECT_EQ(original.getSize(), 0); // Original should still be empty
    EXPECT_EQ(original.get(0), nullptr);
}

TEST_F(CustomStringArrayTest, MoveAssignmentOperator)
{
    std::vector<const char *> initialData1 = {"one", "two"};
    char **c_arr1 = createSampleData(initialData1);
    CustomStringArray arr1(c_arr1, initialData1.size());

    std::vector<const char *> initialData2 = {"alpha", "beta", "gamma"};
    char **c_arr2 = createSampleData(initialData2);
    CustomStringArray arr2(c_arr2, initialData2.size());
    char *arr2Ptr0 = arr2.get(0); // Get pointer before move
    int arr2Size = arr2.getSize();

    arr1 = std::move(arr2); // Invoke move assignment

    // 1. Check arr1 (should have arr2's old state)
    ASSERT_EQ(arr1.getSize(), arr2Size);
    ASSERT_NE(arr1.get(0), nullptr);
    ASSERT_STREQ(arr1.get(0), "alpha");
    ASSERT_EQ(arr1.get(0), arr2Ptr0); // Pointer should be transferred

    // 2. Check arr2 (should be empty)
    EXPECT_EQ(arr2.getSize(), 0);
    EXPECT_EQ(arr2.get(0), nullptr);

    delete[] c_arr1;
    delete[] c_arr2;
}

TEST_F(CustomStringArrayTest, MoveAssignmentToEmpty)
{
    CustomStringArray arr1; // Empty
    std::vector<const char *> initialData2 = {"move", "to", "empty"};
    char **c_arr2 = createSampleData(initialData2);
    CustomStringArray arr2(c_arr2, initialData2.size());
    char *arr2Ptr1 = arr2.get(1);
    int arr2Size = arr2.getSize();

    arr1 = std::move(arr2);

    ASSERT_EQ(arr1.getSize(), arr2Size);
    ASSERT_STREQ(arr1.get(1), "to");
    ASSERT_EQ(arr1.get(1), arr2Ptr1); // Pointer transferred

    EXPECT_EQ(arr2.getSize(), 0);
    EXPECT_EQ(arr2.get(0), nullptr);

    delete[] c_arr2;
}

TEST_F(CustomStringArrayTest, MoveAssignmentFromEmpty)
{
    std::vector<const char *> initialData1 = {"move", "from", "empty"};
    char **c_arr1 = createSampleData(initialData1);
    CustomStringArray arr1(c_arr1, initialData1.size());
    CustomStringArray arr2; // Empty

    arr1 = std::move(arr2); // Assign empty to non-empty via move

    EXPECT_EQ(arr1.getSize(), 0); // arr1 should become empty
    EXPECT_EQ(arr1.get(0), nullptr);
    EXPECT_EQ(arr2.getSize(), 0); // arr2 remains empty
    EXPECT_EQ(arr2.get(0), nullptr);

    delete[] c_arr1;
}

TEST_F(CustomStringArrayTest, MoveAssignmentSelf)
{
    std::vector<const char *> initialData = {"move", "self"};
    char **c_arr = createSampleData(initialData);
    CustomStringArray arr(c_arr, initialData.size());
    
    // Store pointers to check for validity after move
    char *ptr1 = arr.get(0);
    char *ptr2 = arr.get(1);
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);

    // Self-move should be safe but is generally not recommended
    CustomStringArray&& ref = std::move(arr);
    arr = std::move(ref); // Use reference to avoid direct self-move warning

    // Verify object is still in valid state
    ASSERT_EQ(arr.getSize(), 2);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), "move");
    ASSERT_STREQ(arr.get(1), "self");

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, GetMethod)
{
    std::vector<const char *> initialData = {"get", "index", nullptr, "last"};
    char **c_arr = createSampleData(initialData);
    CustomStringArray arr(c_arr, initialData.size());

    // Valid indices
    ASSERT_STREQ(arr.get(0), "get");
    ASSERT_STREQ(arr.get(1), "index");
    ASSERT_EQ(arr.get(2), nullptr);
    ASSERT_STREQ(arr.get(3), "last");

    // Invalid indices
    EXPECT_EQ(arr.get(-1), nullptr);
    EXPECT_EQ(arr.get(4), nullptr);   // One past the end
    EXPECT_EQ(arr.get(100), nullptr); // Far past the end

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, GetMethodEmpty)
{
    CustomStringArray arr;
    EXPECT_EQ(arr.get(0), nullptr);
    EXPECT_EQ(arr.get(-1), nullptr);
}

TEST_F(CustomStringArrayTest, AddMethod)
{
    CustomStringArray arr;

    // Add to empty
    arr.add("first");
    ASSERT_EQ(arr.getSize(), 1);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), "first");

    // Add to non-empty
    arr.add("second");
    ASSERT_EQ(arr.getSize(), 2);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), "first"); // Check previous element still there
    ASSERT_NE(arr.get(1), nullptr);
    ASSERT_STREQ(arr.get(1), "second");

    // Add another
    arr.add("third");
    ASSERT_EQ(arr.getSize(), 3);
    ASSERT_STREQ(arr.get(0), "first");
    ASSERT_STREQ(arr.get(1), "second");
    ASSERT_STREQ(arr.get(2), "third");
}

TEST_F(CustomStringArrayTest, AddMethodMemoryCheck)
{
    // This test tries to ensure add doesn't corrupt previous pointers
    CustomStringArray arr;
    arr.add("one");
    char *ptr1 = arr.get(0); // Get pointer to internal string

    arr.add("two");
    char *ptr2 = arr.get(1);

    // Check if previous pointer is still valid *and points to the same content*
    // Note: The *value* of ptr1 might change if the underlying array is reallocated,
    // but the content it *logically* points to within the CustomStringArray context
    // should remain "one". We access it via get(0).
    ASSERT_EQ(arr.getSize(), 2);
    ASSERT_STREQ(arr.get(0), "one");
    ASSERT_STREQ(arr.get(1), "two");

    // Add again
    arr.add("three");
    ASSERT_EQ(arr.getSize(), 3);
    ASSERT_STREQ(arr.get(0), "one");
    ASSERT_STREQ(arr.get(1), "two");
    ASSERT_STREQ(arr.get(2), "three");
}

// --- Potential Improvements / Bug Fixes Found Via Testing ---
// Note: The following tests might fail with your current code and highlight issues.

TEST_F(CustomStringArrayTest, ConstructorLenBug)
{
    // Your constructor has: int len = strlen(arr[i] + 1);
    // This calculates the length starting from the *second* character.
    // It should be: int len = strlen(arr[i]) + 1;
    std::vector<const char *> initialData = {"abc"};
    char **c_arr = createSampleData(initialData);

    CustomStringArray arr(c_arr, 1);

    // This assertion WILL LIKELY FAIL with the original code.
    // It expects "abc" but might get "bc" or crash depending on memory layout.
    // EXPECT_STREQ(arr.get(0), "abc"); // Fails with strlen(arr[i] + 1)

    // Corrected expectation based on the bug (strlen("abc"+1) -> strlen("bc") -> 2. len=2+1=3)
    // memcpy will copy 3 bytes from "abc" -> 'a','b','c'
    // If the bug is strlen(arr[i] + 1), it calculates len of "bc" (which is 2), adds 1 -> len = 3.
    // Then it allocates `new char[3]`
    // Then `memcpy(dest, src, len)` copies 3 bytes from `arr[i]` ("abc") into `stringArray_[i]`.
    // So it copies 'a', 'b', 'c'. It *misses* the null terminator.
    // EXPECT_EQ(strlen(arr.get(0)), 3); // This might pass by chance or crash
    // Let's just check the size calculation was wrong.
    // A correct implementation would allocate strlen("abc") + 1 = 4 bytes.
    // The buggy one allocates strlen("bc") + 1 = 3 bytes.

    // We can't easily check allocation size directly, but the missing null terminator
    // is the real problem caused by both the strlen and memcpy issues.
    // ASSERT_STREQ will likely fail or crash due to missing '\0'.

    // After fixing strlen to `strlen(arr[i]) + 1` and memcpy to `memcpy(..., len)`:
    ASSERT_EQ(arr.getSize(), 1);
    ASSERT_NE(arr.get(0), nullptr);
    ASSERT_STREQ(arr.get(0), "abc"); // This should pass after fixing the bug.

    delete[] c_arr;
}

TEST_F(CustomStringArrayTest, CopyConstructorLoopBug)
{
    // Your copy constructor has: for (int i = 0; i < size_; ++size_)
    // It increments size_ instead of i in the loop condition.
    std::vector<const char *> initialData = {"a", "b"};
    char **c_arr = createSampleData(initialData);
    CustomStringArray original(c_arr, 2);

    // This will likely crash or have undefined behavior due to the loop bug.
    CustomStringArray copy = original;

    // If the loop bug is fixed(to++ i), this test should pass.ASSERT_EQ(copy.getSize(), 2);
    ASSERT_STREQ(copy.get(0), "a");
    ASSERT_STREQ(copy.get(1), "b");

    // Since the test will likely crash before assertions, we comment them out.
    // The act of constructing 'copy' is the test here for the crash.
    // If it doesn't crash, the bug might be fixed. Add assertions back then.

    delete[] c_arr;
    // GTEST_SKIP() << "Skipping test for Copy Constructor loop bug until fixed, likely crashes.";
}

// Add more tests for edge cases as needed (e.g., very long strings, large arrays if performance matters)
