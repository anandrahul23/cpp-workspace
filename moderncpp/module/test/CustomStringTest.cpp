#include "gtest/gtest.h"
#include "../include/CustomString.h"
#include <cstring>
#include <stdexcept>

TEST(CustomStringTest, DefaultConstructor) {
    CustomString s;
    // Default constructed string should have nullptr internal char* and accessing any index should throw
    EXPECT_THROW(s[0], std::out_of_range);
}
 
TEST(CustomStringTest, ConstructorFromCharPointer) {
    char input[] = "hello";
    CustomString s(input);
    EXPECT_EQ(s[0], 'h');
    EXPECT_EQ(s[1], 'e');
    EXPECT_EQ(s[2], 'l');
    EXPECT_EQ(s[3], 'l');
    EXPECT_EQ(s[4], 'o');
    // With the new size() and c_str() methods, we can write better tests
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s.c_str(), "hello");
}

TEST(CustomStringTest, CopyConstructor) {
    char input[] = "copy";
    CustomString s1(input);
    CustomString s2(s1);
    EXPECT_EQ(s2[0], 'c');
    EXPECT_EQ(s2[1], 'o');
    EXPECT_EQ(s2[2], 'p');
    EXPECT_EQ(s2[3], 'y');
    EXPECT_EQ(s1.size(), 4);
    EXPECT_EQ(s2.size(), 4);
    // Verify deep copy: modifying s1 should not affect s2
    s1[0] = 'x';
    EXPECT_EQ(s1[0], 'x');
    EXPECT_EQ(s2[0], 'c');
}

TEST(CustomStringTest, AssignmentOperator) {
    char input1[] = "first";
    char input2[] = "second";
    CustomString s1(input1);
    CustomString s2(input2);
    s1 = s2;
    EXPECT_EQ(s1[0], 's');
    EXPECT_EQ(s1[1], 'e');
    EXPECT_EQ(s1[2], 'c');
    EXPECT_EQ(s1[3], 'o');
    EXPECT_EQ(s1[4], 'n');
    EXPECT_EQ(s1[5], 'd');
    EXPECT_EQ(s1.size(), 6);
    EXPECT_EQ(s2.size(), 6);
    // Verify deep copy: modifying s2 should not affect s1
    s2[0] = 'z';
    EXPECT_EQ(s2[0], 'z');
    EXPECT_EQ(s1[0], 's');

    // Test assignment to self (should be handled gracefully)
    s1 = s1;
    EXPECT_EQ(s1[0], 's'); // Should remain 's'
}

TEST(CustomStringTest, MoveConstructor) {
    char input[] = "move";
    CustomString s1(input);
    CustomString s2(std::move(s1));
    // Verify content of s2
    EXPECT_EQ(s2[0], 'm'); EXPECT_EQ(s2[1], 'o');
    EXPECT_EQ(s2[2], 'v'); EXPECT_EQ(s2[3], 'e');
    // s1 should be empty now
    EXPECT_EQ(s1.size(), 0);
    EXPECT_THROW(s1[0], std::out_of_range); // s1's internal pointer should be nullptr
}

TEST(CustomStringTest, MoveAssignmentOperator) {
    char input1[] = "abc";
    char input2[] = "xyz";
    CustomString s1(input1);
    CustomString s2(input2);
    // Store original s1's internal pointer to ensure it's deleted
    // (This would require exposing internal `str` or a similar mechanism,
    // but for now, we trust `destroy()` and `operator=` to handle it.)

    s1 = std::move(s2);
    // Verify content of s1
    EXPECT_EQ(s1[0], 'x'); EXPECT_EQ(s1[1], 'y'); EXPECT_EQ(s1[2], 'z');
    // s2 should be empty now
    EXPECT_EQ(s2.size(), 0);
    EXPECT_THROW(s2[0], std::out_of_range); // s2's internal pointer should be nullptr

    // Test self-move assignment (should be safe, though unusual)
    CustomString s3("selfmove");
    s3 = std::move(s3);
    EXPECT_EQ(s3[0], 's'); // Should still contain "selfmove"
    EXPECT_EQ(s3.size(), 8);
}

TEST(CustomStringTest, OutOfRangeThrows) {
    char input[] = "test";
    CustomString s(input);
    EXPECT_THROW(s[100], std::out_of_range); // Accessing index far out of bounds
    EXPECT_THROW(s[4], std::out_of_range);   // Accessing index equal to size (out of bounds)
    // The test for -1 is not valid for size_t, as it will wrap around to a large positive number.
    // The s[100] test already covers large indices.
}

TEST(CustomStringTest, SelfAssignment) {
    char input[] = "self";
    CustomString s(input);
    s = s;
    EXPECT_EQ(s[0], 's'); EXPECT_EQ(s[1], 'e'); EXPECT_EQ(s[2], 'l'); EXPECT_EQ(s[3], 'f');
}

TEST(CustomStringTest, ConstAccessOperator) {
    const char input[] = "const";
    const CustomString s(input); // Create a const CustomString
    EXPECT_EQ(s[0], 'c');
    EXPECT_EQ(s[4], 't');
    EXPECT_THROW(s[5], std::out_of_range); // Const version should also throw
}

TEST(CustomStringTest, ModifyViaOperator) {
    char input[] = "hello";
    CustomString s(input);
    s[0] = 'j';
    s[4] = 'a';
    EXPECT_EQ(s[0], 'j');
    EXPECT_EQ(s[1], 'e');
    EXPECT_EQ(s[2], 'l');
    EXPECT_EQ(s[3], 'l');
    EXPECT_EQ(s[4], 'a');
}

TEST(CustomStringTest, ConstructorFromNullptr) {
    CustomString s(nullptr);
    EXPECT_THROW(s[0], std::out_of_range);
}

TEST(CustomStringTest, AssignmentFromNullptr) {
    CustomString s("initial");
    CustomString s_null(nullptr);
    s = s_null; // Assign from a CustomString that holds nullptr
    EXPECT_THROW(s[0], std::out_of_range);

    CustomString s2("another");
    // This would require an `operator=(const char*)` or similar, which is not present.
    // If it were, we'd test: s2 = nullptr; EXPECT_THROW(s2[0], std::out_of_range*);
}

TEST(CustomStringTest, DestructorHandlesNullptr) {
    // This test primarily checks for crashes/memory errors when a CustomString
    // holding a nullptr is destructed. Valgrind or ASan would be ideal for this.
    CustomString s; // Default constructor sets str to nullptr
    // No explicit assertions, just ensuring no crash on destruction
}

TEST(CustomStringTest, DestructorHandlesAllocatedMemory) {
    // Similar to above, relies on memory tools for full verification.
    // Ensures `delete[] str` is called correctly.
    CustomString s("short");
    // No explicit assertions, just ensuring no crash on destruction
}
TEST(CustomStringTest, SizeAndCStrMethods) {
    CustomString s1("hello");
    EXPECT_EQ(s1.size(), 5);
    EXPECT_STREQ(s1.c_str(), "hello");

    CustomString s2("");
    EXPECT_EQ(s2.size(), 0);
    EXPECT_STREQ(s2.c_str(), "");

    CustomString s3(nullptr);
    EXPECT_EQ(s3.size(), 0);
    EXPECT_EQ(s3.c_str(), nullptr); // c_str() should return nullptr for null string
}

TEST(CustomStringTest, DereferenceOperator) {
    CustomString s("test");
    EXPECT_STREQ(*s, "test");

    CustomString s_empty("");
    EXPECT_STREQ(*s_empty, "");

    CustomString s_null(nullptr);
    EXPECT_EQ(*s_null, nullptr); // Dereferencing a null string should return nullptr
}

TEST(CustomStringTest, StreamInsertionOperator) {
    CustomString s("hello world");
    std::stringstream ss;
    ss << s;
    EXPECT_EQ(ss.str(), "hello world");

    CustomString s_empty("");
    std::stringstream ss_empty;
    ss_empty << s_empty;
    EXPECT_EQ(ss_empty.str(), "");

    CustomString s_null(nullptr);
    std::stringstream ss_null;
    // Expecting an exception for null string as per CustomString.cpp
    EXPECT_THROW(ss_null << s_null, std::runtime_error);
}

TEST(CustomStringTest, PrefixIncrementOperator) {
    CustomString s("abc");
    ++s; // Should become "bc"
    EXPECT_EQ(s.size(), 2);
    EXPECT_STREQ(s.c_str(), "bc");

    ++s; // Should become "c"
    EXPECT_EQ(s.size(), 1);
    EXPECT_STREQ(s.c_str(), "c");

    ++s; // Should become ""
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");

    
    ++s; // Should remain ""
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");

    CustomString s_empty("");
    ++s_empty; // Should remain ""
    EXPECT_EQ(s_empty.size(), 0);
    EXPECT_STREQ(s_empty.c_str(), "");

    CustomString s_null(nullptr);
    // Incrementing a null string should not crash and remain null/empty
    ++s_null;
    EXPECT_EQ(s_null.size(), 0);
    EXPECT_EQ(s_null.c_str(), nullptr);
}

TEST(CustomStringTest, PostfixIncrementOperator) {
    CustomString s("abc");
    CustomString old_s = s++; // old_s should be "abc", s should be "bc"
    EXPECT_EQ(old_s.size(), 3);
    EXPECT_STREQ(old_s.c_str(), "abc");
    EXPECT_EQ(s.size(), 2);
    EXPECT_STREQ(s.c_str(), "bc");

    old_s = s++; // old_s should be "bc", s should be "c"
    EXPECT_EQ(old_s.size(), 2);
    EXPECT_STREQ(old_s.c_str(), "bc");
    EXPECT_EQ(s.size(), 1);
    EXPECT_STREQ(s.c_str(), "c");

    old_s = s++; // old_s should be "c", s should be ""
    EXPECT_EQ(old_s.size(), 1);
    EXPECT_STREQ(old_s.c_str(), "c");
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");

    old_s = s++; // old_s should be "", s should be ""
    EXPECT_EQ(old_s.size(), 0);
    EXPECT_STREQ(old_s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.c_str(), "");
}

TEST(CustomStringTest, AdditionOperator) {
    CustomString s1("hello");
    CustomString s2("world");
    CustomString s3 = s1 + s2;
    EXPECT_EQ(s3.size(), 10);
    EXPECT_STREQ(s3.c_str(), "helloworld");

    CustomString s4("");
    CustomString s5("test");
    CustomString s6 = s4 + s5;
    EXPECT_EQ(s6.size(), 4);
    EXPECT_STREQ(s6.c_str(), "test");

    CustomString s7("test");
    CustomString s8("");
    CustomString s9 = s7 + s8;
    EXPECT_EQ(s9.size(), 4);
    EXPECT_STREQ(s9.c_str(), "test");

    CustomString s10("");
    CustomString s11("");
    CustomString s12 = s10 + s11;
    EXPECT_EQ(s12.size(), 0);
    EXPECT_STREQ(s12.c_str(), "");
}

TEST(CustomStringTest, AdditionOperatorWithNullptr) {
    CustomString s1("hello");
    CustomString s_null(nullptr);

    // Adding a null string should throw std::logic_error as per CustomString.cpp
    EXPECT_THROW(s1 + s_null, std::logic_error);
    EXPECT_THROW(s_null + s1, std::logic_error);
    EXPECT_THROW(s_null + s_null, std::logic_error);
}
