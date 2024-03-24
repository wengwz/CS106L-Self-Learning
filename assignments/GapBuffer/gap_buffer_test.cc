/*
 * CS 106L Assignment 3: Gap Buffer (version 2.21)
 * Test Harness written by Avery Wang (awvry952@stanford.edu)
 * Feb. 28, 2020 (Updated)
 */


#include <iostream>
#include <vector>
#include <chrono>
#include <sstream>
#include "gap_buffer.h"
#include "gtest/gtest.h"

#define TEST1_ENABLED 1
#define TEST2_ENABLED 0
#define TEST3_ENABLED 0
#define TEST4_ENABLED 0
#define TEST5_ENABLED 0
#define TEST6_ENABLED 0
#define TEST7_ENABLED 0
#define TEST8_ENABLED 0
#define TEST9_ENABLED 0

#if TEST1_ENABLED
/*
 * Inserts characters a through g, then uses at to check
 * that the characters are written there.
 *
 * Buffer: [ a b c d e f g ]
 */
TEST(GapBufferTest, TEST1A_INSERT_AT_CURSOR_BASIC) {

    GapBuffer<char> buf;
    for (char ch = 'a'; ch < 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }

    // buf.debug() are useful if you can't figure out what's the problem.
    for (char ch = 'a'; ch < 'g'; ch++) {
        ASSERT_EQ(ch, buf.at(ch - 'a'));
    }
}

/*
 * Inserts characters a through g, then deletes
 *
 * Buffer: [ a b c d e f g ] -> [ a b c d e ] -> [ a b c d e x y z]
 */
TEST(GapBufferTest, TEST1B_INSERT_THEN_DELETE_BASIC) {

    GapBuffer<char> buf;
    for (char ch = 'a'; ch <= 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }
    buf.delete_at_cursor();
    buf.delete_at_cursor();
    char x = 'x'; // have to make these l-values so we don't
    char y = 'y'; // accidently call the r-value version of insert_at_cursor.
    char z = 'z';
    buf.insert_at_cursor(x);
    buf.insert_at_cursor(y);
    buf.insert_at_cursor(z);

    for (char ch = 'a'; ch < 'e'; ch++) {
        ASSERT_EQ(ch, buf.at(ch - 'a'));
    }
    ASSERT_EQ(8, buf.size());
    ASSERT_EQ('x', buf.at(5));
    ASSERT_EQ('y', buf.at(6));
    ASSERT_EQ('z', buf.at(7));
    ASSERT_EQ(8, buf.size());
}

/*
 * Inserts characters abc, then tries ot overwrite c with d, then e
 * Tests
 *
 * Buffer: [ a b c ] -> [ a b e ]
 */
TEST(GapBufferTest, TEST1C_EDIT_AT_BASIC) {

    GapBuffer<char> buf;
    char a = 'a';
    char b = 'b';
    char c = 'c';
    buf.insert_at_cursor(a);
    buf.insert_at_cursor(b);
    buf.insert_at_cursor(c);
    ASSERT_EQ(a, buf.at(0));
    ASSERT_EQ(b, buf.at(1));
    ASSERT_EQ(c, buf.at(2));
    buf.at(2) = 'd';
    buf.at(2) = 'e';
    ASSERT_EQ(a, buf.at(0));
    ASSERT_EQ(b, buf.at(1));
    ASSERT_EQ('e', buf.at(2));
}

/*
 * Tests insert and move.
 *
 * Buffer: [ a b c d e f g ]
 */
TEST(GapBufferTest, TEST1D_INSERT_MOVE_CURSOR_AT_BASIC) {

    GapBuffer<char> buf;
    for (char ch = 'a'; ch < 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }

    for (char ch = 'f'; ch >= 'a'; ch--) {
        buf.move_cursor(-1);
        ASSERT_EQ(ch, buf.get_at_cursor());
    }
}

/*
 * Verifies correct positioning of cursor after insert and delete
 *
 * Buffer: [ a a ] -> [ a ] -> [ a b ] -> [ a b b ] -> [ a b ] -> etc.
 * End result: [ a b c d e f g h i j ]
 */
TEST(GapBufferTest, TEST1E_INSERT_DELETE_MIXED_HARD) {

    GapBuffer<char> buf;
    for (int i = 0; i < 10; ++i) {
        char ch = 'a' + i;
        buf.insert_at_cursor(ch);
        buf.insert_at_cursor(ch);
        buf.delete_at_cursor();
    }

    for (int i = 0; i < 10; ++i) {
        char ch = 'a' + i;
        ASSERT_EQ(ch, buf.at(i));
    }
}

/*
 * Verifies that delete_at_cursor when cursor_index = 0 is a no-op.
 */
TEST(GapBufferTest, TEST1F_DELETE_EMPTY_BASIC) {

    GapBuffer<char> buf;
    for (size_t i = 0; i < 10; ++i) {
        char ch = 'a' + i;
        buf.insert_at_cursor(ch);
    }

    buf.move_cursor(-1*buf.size());
    buf.delete_at_cursor(); // should be a no-op.
    ASSERT_EQ(10, buf.size());
    buf.move_cursor(buf.size());
    ASSERT_FALSE(buf.empty());

    for (size_t i = 0; i < 10; ++i) {
        buf.delete_at_cursor();
        ASSERT_EQ(9-i, buf.size());
    }
    ASSERT_TRUE(buf.empty());
}

// /*
//  * Interweaves insert and move cursor calls.
//  */
TEST(GapBufferTest, TEST1G_INSERT_MOVE_MIXED) {

    GapBuffer<char> buf;
    for (char ch = 'a'; ch <= 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }
    for (char ch = 'g'; ch > 'a'; ch--) {
        buf.insert_at_cursor(ch);
        buf.move_cursor(-1);
    }
    buf.move_cursor(-1);
}

// /*
//  * Checks if move_cursor by large amounts work correctly.
//  */
TEST(GapBufferTest, TEST1H_MOVE_CURSOR_JUMP) {

    GapBuffer<char> buf;
    for (char ch = 'a'; ch <= 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }
    buf.move_cursor(-1);
    for (char ch = 'g'; ch >= 'c'; ch -= 3) {
        ASSERT_EQ(ch, buf.get_at_cursor());
        buf.move_cursor(-3);
    }
}

// /*
//  * Tests reserve by inserting past the default 10 elements.
//  * Uses move and at to verify all elements are correct.
//  */
TEST(GapBufferTest, TEST1I_RESERVE_BASIC) {
    
    GapBuffer<char> buf;
    std::vector<char> vec;
    for (char ch = 'a'; ch <= 'o'; ++ch) {
        buf.insert_at_cursor(ch);
        vec.push_back(ch);
        ASSERT_EQ(buf.size(), vec.size());
    }
    buf.move_cursor(-14);
    for (size_t i = 0; i < 14; i++) {
        ASSERT_EQ(buf.at(i), vec[i]);
        buf.move_cursor(1);
    }
}

// /*
//  * Calls reserve explicitly and checks for off-by-one errors.
//  */
TEST(GapBufferTest, TEST1J_RESERVE_EDGE) {

    GapBuffer<char> buf;
    char h = 'H';
    for (size_t i = 0; i < 10; ++i) {
        buf.insert_at_cursor(h);
    }

    buf.reserve(0);
    buf.reserve(9);
    char exclaim = '!';
    buf.insert_at_cursor(exclaim);
    buf.reserve(10);
    buf.reserve(11);
    buf.reserve(0);
    ASSERT_EQ(11, buf.size());
    ASSERT_EQ(11, buf.cursor_index());
}

// /*
//  * Calls reserve first, then starts begins filling up elements.
//  * Fun fact, this test caught a very subtle bug in the starter code
//  * (indexing issue inside move_cursor) that only appeared
//  * if you called reserve explicitly!
//  *
//  * Lesson: Write unit tests!
//  */
TEST(GapBufferTest, TEST1K_RESERVE_EXPLICT_STRESS) {
    
    const size_t stress_size = 10000;
    GapBuffer<size_t> buf;
    buf.reserve(stress_size);
    for (size_t i = 0; i < stress_size; ++i) {
        buf.insert_at_cursor(i);
        buf.move_cursor(-1);
    }

    ASSERT_EQ(buf.size(), stress_size);
    for (size_t i = 0; i < stress_size; ++i) {
        ASSERT_EQ(buf.get_at_cursor(), stress_size-1-i);
        buf.move_cursor(1);
    }
}
#endif

#if TEST2_ENABLED
// /*
//  * Declares a non-const buffer and verifies that
//  * at and get_at_cursor returns references.
//  *
//  * Then declares a const reference to that buffer
//  * and verifies that at and get_at_cursor returns const_references.
//  */
TEST(GapBufferTest, TEST2A_CONST_TEST) {
    GapBuffer<int> buf;
    for (size_t i = 0; i < 20; ++i) {
        buf.insert_at_cursor(i);
    }
    buf.move_cursor(-20);
    ASSERT_EQ(buf.get_at_cursor(), 0);
    buf.get_at_cursor() = 100;
    ASSERT_EQ(buf.get_at_cursor(), 100);

    ASSERT_EQ(buf.at(10), 10);
    buf.at(10) = -10;
    ASSERT_EQ(buf.at(10), -10);

    const auto& buf_ref = buf;
    ASSERT_EQ(buf_ref.at(10), -10);
    ASSERT_EQ(buf_ref.size(), 20);
    ASSERT_EQ(buf_ref.empty(), false);
    ASSERT_EQ(buf_ref.get_at_cursor(), 100);
}
#endif

#if TEST3_ENABLED
// /*
//  * Tests the fill constructor by creating GapBuffer of 200 elements
//  */
TEST(GapBufferTest, TEST3A_FILL_CONSTRUCTOR_TEST_BASIC) {
    GapBuffer<char> buf(200, 'c');
    for (int i = 0; i < 200; ++i) {
        ASSERT_EQ(buf.at(i), 'c');
    }
    ASSERT_EQ(buf.cursor_index(), 200);
}

// /*
//  * Tests the edge case when you start with 0 or one element.
//  * Tests insertion to ensure that is still possible.
//  */
TEST(GapBufferTest, TEST3B_FILL_CONSTRUCTOR_EDGE) {
    GapBuffer<char> buf_zero(0, '%');
    ASSERT_EQ(buf_zero.size(), 0);
    ASSERT_EQ(buf_zero.cursor_index(), 0);

    for (int i = 0; i < 20; ++i) {
        char ch = 'a' + i;
        buf_zero.insert_at_cursor(ch);
    }

    for (int i = 0; i < 20; ++i) {
        ASSERT_EQ(buf_zero.at(i), ('a' + i));
    }

    GapBuffer<char> buf_one(1, '%');
    ASSERT_EQ(buf_one.size(), 1);
    ASSERT_EQ(buf_one.cursor_index(), 1);
    for (int i = 0; i < 20; ++i) {
        char ch = 'a' + i;
        buf_one.insert_at_cursor(ch);
    }

    ASSERT_EQ(buf_one.at(0), '%');
    for (int i = 0; i < 20; ++i) {
        ASSERT_EQ(buf_one.at(i+1), ('a' + i));
    }

    GapBuffer<char> buf_default(15);
    for (int i = 0; i < 15; ++i) {
        ASSERT_EQ(buf_default[i], char()); // check equal to default value
    }
}

// /*
//  * Tests basic use of the operator []
//  * Note: remove the //QSKIP statement after implementing the operator
//  * QSKIP prevents compiler errors even if you haven't implemented it.
//  */
TEST(GapBufferTest, TEST3C_INDEX_OP_BASIC){
    GapBuffer<char> buf;
    for (char ch = 'a'; ch < 'g'; ch++) {
        buf.insert_at_cursor(ch);
    }

    for (char ch = 'a'; ch < 'g'; ch++) {
        ASSERT_EQ(ch, buf[ch - 'a']);
    }
}

// /*
//  * Does the same test as TEST2A, but with operator[].
//  */
TEST(GapBufferTest, TEST3D_INDEX_OP_CONST) {
    GapBuffer<int> buf;
    for (size_t i = 0; i < 20; ++i) {
        buf.insert_at_cursor(i);
    }
    buf.move_cursor(-20);
    ASSERT_EQ(buf.get_at_cursor(), 0);
    buf.get_at_cursor() = 100;
    ASSERT_EQ(buf.get_at_cursor(), 100);

    ASSERT_EQ(buf[10], 10);
    buf.at(10) = -10;
    ASSERT_EQ(buf[10], -10);

    const auto& buf_ref = buf;
    ASSERT_EQ(buf_ref[10], -10);
    ASSERT_EQ(buf_ref.size(), 20);
    ASSERT_EQ(buf_ref.empty(), false);
    ASSERT_EQ(buf_ref.get_at_cursor(), 100);
}

// /*
//  * Verifies the reference returned by operator[] is correct.
//  */
TEST(GapBufferTest, TEST3E_INDEX_OP_REF_ADVANCED) {
    GapBuffer<int> buf;
    for (size_t i = 0; i < 10; ++i) {
        buf.insert_at_cursor(i);
    }
    auto val5 = buf[5];
    auto& val7 = buf[7];
    const auto& val8 = buf[8];
    val5 = -5;
    val7 = -7;
    (void) val8; // suppress warning
    ASSERT_EQ(buf[5], 5);
    ASSERT_EQ(buf[7], -7);
    ASSERT_EQ(buf[8], 8);
}

// /*
//  * Verifies that operator << for ostreams is correct.
//  */
TEST(GapBufferTest, TEST3F_OSTREAM_INSERTION_BASIC) {
    GapBuffer<int> buf;
    for (size_t i = 1; i <= 5; ++i) {
        buf.insert_at_cursor(i);
    }

    std::ostringstream oss1;
    oss1 << buf;
    ASSERT_EQ(oss1.str(), "{1, 2, 3, 4, 5^}");
    buf.move_cursor(-3);

    std::ostringstream oss2;
    oss2 << buf;
    ASSERT_TRUE(oss2.str() == "{1, 2, ^3, 4, 5}" || oss2.str() == "{1, 2,^3, 4, 5}");

    buf.move_cursor(-2);

    std::ostringstream oss3;
    oss3 << buf;
    ASSERT_EQ(oss3.str(), "{^1, 2, 3, 4, 5}");
}

// /*
//  * Verifies a few edge cases for operator<<.
//  */
TEST(GapBufferTest, TEST3G_OSTREAM_INSERTION_EDGE) {
    GapBuffer<int> new_buf;
    std::ostringstream oss4;
    oss4 << new_buf;
    ASSERT_EQ(oss4.str(), "{^}");

    int i = 1;
    new_buf.insert_at_cursor(i);
    std::ostringstream oss5;
    oss5 << new_buf;
    ASSERT_EQ(oss5.str(), "{1^}");

    new_buf.move_cursor(-1);
    std::ostringstream oss6;
    oss6 << new_buf;
    ASSERT_TRUE(oss6.str() == "{^1}" || oss6.str() == "{^1}"); // not strict on spaces before ^

    new_buf.move_cursor(1);
    int j = 2;
    new_buf.insert_at_cursor(j);
    std::ostringstream oss7;
    oss7 << new_buf;
    ASSERT_EQ(oss7.str(), "{1, 2^}");

    new_buf.move_cursor(-1);
    std::ostringstream oss8;
    oss8 << new_buf;
    ASSERT_TRUE(oss8.str() == "{1, ^2}" || oss8.str() == "{1,^2}"); // not strict on spaces before ^
}

// /*
//  * Verifies that the operator << can be chained.
//  */
TEST(GapBufferTest, TEST3H_OSTREAM_INSERTION_CHAIN) {
    GapBuffer<int> new_buf;
    std::ostringstream oss4;
    oss4 << new_buf << new_buf << new_buf;
    ASSERT_EQ(oss4.str(), "{^}{^}{^}");
}

// /*
//  * Verifies operator== and operator!= correctly implemented.
//  */
TEST(GapBufferTest, TEST3I_EQ_UNEQ_OP) {
    GapBuffer<char> buf1;
    GapBuffer<char> buf2;
    GapBuffer<char> buf3;
    GapBuffer<char> buf4;
    GapBuffer<char> buf5;
    GapBuffer<char> buf6;
    std::string avery = "avery";
    std::string anna = "anna";
    std::string ali = "ali";
    std::string a = "a";
    std::string empty = "";

    for (const auto& ch : avery) buf1.insert_at_cursor(ch);
    for (const auto& ch : anna) buf2.insert_at_cursor(ch);
    for (const auto& ch : ali) buf3.insert_at_cursor(ch);
    for (const auto& ch : a) buf4.insert_at_cursor(ch);
    for (const auto& ch : empty) buf5.insert_at_cursor(ch);
    for (const auto& ch : avery) buf6.insert_at_cursor(ch);

    ASSERT_TRUE(buf1 == buf6);
    ASSERT_TRUE(buf1 != buf2);
    ASSERT_TRUE(buf1 != buf3);
    ASSERT_TRUE(buf1 != buf4);
    ASSERT_TRUE(buf1 != buf5);
    ASSERT_TRUE(buf2 != buf3);
    ASSERT_TRUE(buf5 != buf6);
    ASSERT_TRUE(buf1 == buf1);
    ASSERT_TRUE(buf2 == buf2);
    ASSERT_TRUE(buf5 == buf5);
}

// /*
//  * Verifies comparison operators (<, >, <=, >=) correctly implemented.
//  */
TEST(GapBufferTest, TEST3J_CMP_OP) {
    GapBuffer<char> buf1;
    GapBuffer<char> buf2;
    GapBuffer<char> buf3;
    GapBuffer<char> buf4;
    GapBuffer<char> buf5;
    GapBuffer<char> buf6;
    std::string avery = "avery";
    std::string anna = "anna";
    std::string ali = "ali";
    std::string a = "a";
    std::string empty = "";

    for (const auto& ch : avery) buf1.insert_at_cursor(ch);
    for (const auto& ch : anna) buf2.insert_at_cursor(ch);
    for (const auto& ch : ali) buf3.insert_at_cursor(ch);
    for (const auto& ch : a) buf4.insert_at_cursor(ch);
    for (const auto& ch : empty) buf5.insert_at_cursor(ch);
    for (const auto& ch : avery) buf6.insert_at_cursor(ch);

    ASSERT_FALSE(buf1 < buf6);
    ASSERT_FALSE(buf1 > buf6);
    ASSERT_TRUE(buf1 <= buf6);
    ASSERT_TRUE(buf1 > buf2);
    ASSERT_TRUE(buf1 > buf3);
    ASSERT_TRUE(buf1 > buf4);
    ASSERT_TRUE(buf1 > buf5);
    ASSERT_TRUE(buf2 > buf3);
    ASSERT_TRUE(buf5 <= buf6);
    ASSERT_FALSE((buf1 > buf1) || (buf1 < buf1));
    ASSERT_FALSE((buf2 > buf2) || (buf2 < buf2));
    ASSERT_FALSE((buf5 > buf5) || (buf5 < buf5));
}
#endif

#if TEST5_ENABLED
// /*
//  * Tests the function of the for-each loop, which uses your basic iterator operators.
//  */
TEST(GapBufferTest, TEST5A_ITER_BASIC) {
    std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    GapBuffer<int> buf;
    for (int val : vec) {
        buf.insert_at_cursor(val);
    }

    size_t current_index = 0;
    for (int val : buf) {
        ASSERT_EQ(val, vec[current_index++]);
    }

    // you didn't go out of bounds, did you?
    ASSERT_EQ(current_index, vec.size());
}

// /*
//  * Tests that the dereference operator (*) is
//  * returning a reference to the element, so changing
//  * the element when looping by reference actually changes the element.
//  */
TEST(GapBufferTest, TEST5B_ITER_DEREFERENCE) {
    std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    GapBuffer<int> buf;
    for (int val : vec) {
        buf.insert_at_cursor(val);
    }

    for (int& val : buf) {
        val = -1; // should change all values to -1
    }

    for (int val : buf) {
        ASSERT_EQ(val, -1);
    }
}

// /*
//  * Tests a while loop of an iterator moving backwards from the cursor.
//  */
TEST(GapBufferTest, TEST5C_ITER_BIDIRECTIONAL) {
    std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    GapBuffer<int> buf;
    for (int val : vec) {
        buf.insert_at_cursor(val);
    }

    buf.move_cursor(-1);
    size_t current_index = 8;

    auto iter = buf.cursor();
    while (iter != buf.begin()) {
        --iter;
        ASSERT_EQ(*iter, vec[current_index--]);
    }
    ASSERT_EQ(*iter, vec[0]);
}

// /*
//  * Tests the operators specific to random-access iterators:
//  * +, -, +=, and -=, which you wrote.
//  */
TEST(GapBufferTest, TEST5D_ITERATOR_EDGE) {
    GapBuffer<int> empty;
    ASSERT_TRUE(empty.begin() == empty.end());
    ASSERT_TRUE(empty.begin() == empty.cursor());

    for (int val : empty) {
        FAIL(); // This should never run!
    }
}

// /*
//  * Tests the operators specific to random-access iterators:
//  * +, -, +=, and -=, which youwrote.
//  */
TEST(GapBufferTest, TEST5E_ITERATOR_RANDOM_ACCESS) {
    std::vector<int> vec {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    GapBuffer<int> buf;
    for (int val : vec) {
        buf.insert_at_cursor(val);
    }

    ASSERT_EQ(buf.cursor(), buf.end()); // cursor points to after 12 (end)
    buf.move_cursor(-6); // cursor points to 7
    auto iter = buf.cursor(); // points to 7
    ASSERT_EQ(*iter, 7);
    auto j = iter - 3; // points to 4
    ASSERT_EQ(*j, 4);
    auto k = iter + 3; // points to 10
    auto m = 3 + iter; // tests symmetry
    ASSERT_EQ(*k, 10);
    ASSERT_EQ(*m, 10);
    j += 2; // points to 6
    ASSERT_EQ(*j, 6);
    k -= 2; // points to 8
    ASSERT_EQ(*k, 8);
}

// /*
//  * If you implemented all the operators correctly...std::sort magically works!
//  *
//  * Wait...you say in disbelief. std::sort knows can sort even with the gap?
//  * YES!!! - your iterators abstract away the containers, and the algorithms
//  * work purely on the iterators.
//  */
TEST(GapBufferTest, TEST5F_ITERATOR_ADVANCED_SORT) {
    std::vector<int> vec{5, 2, 6, 3, 9, 0, 1, 8, 4, 7};
    GapBuffer<int> buf;
    for (int val : vec) {
        buf.insert_at_cursor(val);
    }

    std::sort(vec.begin(), vec.end());
    std::sort(buf.begin(), buf.end());

    // /* Mind-blown */
    size_t current_index = 0;
    for (int val : buf) {
        ASSERT_EQ(val, vec[current_index++]);
    }
    // you didn't go out of bounds, did you?
    ASSERT_EQ(current_index, vec.size());
}
#endif

#if TEST6_ENABLED
// /*
//  * Tests the initializer_list constructor.
//  */
TEST(GapBufferTest, TEST6A_INITIALIZER_LIST_BASIC) {
    GapBuffer<int> buf{1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(buf[i], i+1);
    }
    ASSERT_EQ(buf.cursor_index(), buf.size());
}

// /*
//  * Tests an edge case for the initializer_list constructor.
//  */
TEST(GapBufferTest, TEST6B_INITIALIZER_LIST_EDGE) {
    GapBuffer<int> buf_empty{};
    ASSERT_EQ(buf_empty.size(), 0);
    ASSERT_EQ(buf_empty.cursor_index(), 0);

    GapBuffer<int> buf{0};
    ASSERT_EQ(buf.size(), 1);

    for (int i = 1; i < 20; ++i) {
        buf.insert_at_cursor(i);
    }

    for (int i = 0; i < 20; ++i) {
        ASSERT_EQ(buf[i], i);
    }
    ASSERT_EQ(buf.cursor_index(), buf.size());
}

// /*
//  * Tests basic behavior for the copy constructor.
//  */
TEST(GapBufferTest, TEST6C_COPY_CONSTRUCTOR_BASIC) {
    GapBuffer<int> buf1{1, 2, 3};
    GapBuffer<int> buf2 = buf1; // constructor

    // Make sure everything is copied correctly
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf2[i], i+1);
    }
    ASSERT_EQ(buf1.cursor_index(), buf2.cursor_index());

    // Try changing buf2 - it should be a deep copy
    buf2[0] = 5;
    buf2[1] = 6;
    buf2[2] = 7;
    int eight = 8;
    buf2.insert_at_cursor(eight);

    // buf1 should remain unchanged
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf1[i], i+1);
    }
    ASSERT_EQ(buf1.cursor_index(), 3);
}

// /*
//  * Tests basic behavior for the copy assignment operator.
//  */
TEST(GapBufferTest, TEST6D_COPY_ASSIGNMENT_BASIC) {
    GapBuffer<int> buf1{1, 2, 3};
    GapBuffer<int> buf2{4};
    buf2 = buf1; // assignment

    // Make sure everything is copied correctly
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf2[i], i+1);
    }
    ASSERT_EQ(buf1.cursor_index(), buf2.cursor_index());

    // Try changing buf2 - it should be a deep copy
    buf2[0] = 5;
    buf2[1] = 6;
    buf2[2] = 7;

    // buf1 should remain unchanged
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf1[i], i+1);
    }
    ASSERT_EQ(buf1.cursor_index(), 3);
}

// /*
//  * Tests copy constructor and assignment operator for a few edge cases.
//  */
TEST(GapBufferTest, TEST6E_COPY_EDGE) {
    GapBuffer<int> buf1{1};
    GapBuffer<int> buf2 = buf1;
    int two = 2;
    buf1.insert_at_cursor(two);
    ASSERT_EQ(buf1[0], 1);
    ASSERT_EQ(buf1[1], 2);
    ASSERT_EQ(buf2[0], 1);
    ASSERT_EQ(buf2.size(), 1);

    GapBuffer<int> buf3;
    GapBuffer<int> buf4(buf3);
    int four = 4;
    buf4.insert_at_cursor(four);

    // Try some awful things with assignment.
    buf1 = buf1;
    ASSERT_EQ(buf1.size(), 2);
    buf2 = (buf2 = buf2) = buf2;
    ASSERT_EQ(buf2.size(), 1);
    buf3 = (buf4 = buf3) = buf4;
    ASSERT_EQ(buf3.size(), 0);
    ASSERT_EQ(buf4.size(), 0);
    (buf1 = buf1 = buf1 = buf1) = (buf1 = buf1) = buf1;
    ASSERT_EQ(buf1.size(), 2);
    // still alive? good work!
}

// /*
//  * Tests the comparison operators by placing buffers into a set
//  * which requires the comparison operators correctly implemented.
//  *
//  * Note: originally we put this test case in part 3.
//  * Why did we move it here?
//  *
//  * Putting things into a set requires overloading the comparison
//  * operators, which was why we wanted to put the test in part 3.
//  * However, it turns out that when you insert something into
//  * a set, you need to create a copy of that object.
//  * You do that in the copy constructor.
//  *
//  * That's the reason if you try running this test after only
//  * completing part 3, you get a double free error.
//  */
TEST(GapBufferTest, TEST6F_COMPARISON_OPERATOR_SET) {
    GapBuffer<char> buf1{'a','v','e','r','y'};
    GapBuffer<char> buf2{'a','n','n','a'};
    GapBuffer<char> buf3{'a','l','i'};
    GapBuffer<char> buf4{'a'};
    GapBuffer<char> buf5{};
    GapBuffer<char> buf6{'a','v','e','r','y'};

    std::set<GapBuffer<char>> bufs {buf1, buf2, buf3, buf4, buf5, buf6};
    auto iter = bufs.begin();
    ASSERT_EQ(*iter++, buf5);
    ASSERT_EQ(*iter++, buf4);
    ASSERT_EQ(*iter++, buf3);
    ASSERT_EQ(*iter++, buf2);
    ASSERT_EQ(*iter++, buf1);
    ASSERT_EQ(iter, bufs.end());
    ASSERT_EQ(bufs.size(), 5);
}
#endif

#if TEST7_ENABLED
// /*
//  * Tests basic behavior of the move constructor.
//  */
TEST(GapBufferTest, TEST7A_MOVE_CONSTRUCTOR_BASIC) {
    GapBuffer<int> buf1(10, 1);
    GapBuffer<int> buf2 = std::move(buf1); // constructor

    // Make sure everything is copied correctly
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf2[i], 1);
    }
}

TEST(GapBufferTest, TEST7B_MOVE_CONSTRUCTOR_TIME) {
    GapBuffer<int> buf3(10000, 1);
    auto start = std::chrono::high_resolution_clock::now();

    GapBuffer<int> buf4 = std::move(buf3);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    ASSERT_TRUE(elapsed.count() < 10) << "Move constructor must be instantaneous";

    for (size_t i = 0; i < buf4.size(); ++i) {
        ASSERT_EQ(buf4[i], 1); // just to make sure you didn't cheat :)
    }
}

// /*
//  * Tests the basic behavior the move assignment operator.
//  */
TEST(GapBufferTest, TEST7C_MOVE_ASSIGNMENT_BASIC) {
    GapBuffer<int> buf1(10, 1);
    GapBuffer<int> buf2;
    buf1.move_cursor(-3);
    buf2 = std::move(buf1); // assignment

    // Make sure everything is copied correctly
    for (size_t i = 0; i < buf1.size(); ++i) {
        ASSERT_EQ(buf2[i], 1);
    }
    ASSERT_EQ(buf2.cursor_index(), 10-3);
}

// /*
//  * Tests the efficiency of the move assignment operator.
//  * i.e. you should not be copying any elements.
//  */
TEST(GapBufferTest, TEST7D_MOVE_ASSIGNMENT_TIME) {
    GapBuffer<int> buf3(10000, 1);
    auto start = std::chrono::high_resolution_clock::now();
    GapBuffer<int> buf4;
    buf4 = std::move(buf3);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    ASSERT_TRUE(elapsed.count() < 10) << "Move assignment must be instantaneous";

    for (size_t i = 0; i < buf4.size(); ++i) {
        ASSERT_EQ(buf4[i], 1); // just to make sure you didn't cheat :)
    }
}

// /*
//  * Tests the move constructor and move assignment operator
//  * in a few edge cases.
//  */
TEST(GapBufferTest, TEST7E_MOVE_EDGE) {
    GapBuffer<int> buf1{1};
    GapBuffer<int> buf2 = std::move(buf1);
    int two = 2;
    buf2.insert_at_cursor(two);
    ASSERT_EQ(buf2[0], 1);
    ASSERT_EQ(buf2[1], 2);
    ASSERT_EQ(buf2.size(), 2);
    buf1 = std::move(buf2);
    ASSERT_EQ(buf1[0], 1);
    ASSERT_EQ(buf1[1], 2);
    ASSERT_EQ(buf1.size(), 2);
    
    GapBuffer<int> buf3;
    GapBuffer<int> buf4(std::move(buf3));
    int four = 4;
    buf4.insert_at_cursor(four);

    // Try some awful things with assignment.
    buf4 = std::move(buf4);
    ASSERT_EQ(buf4.size(), 1);
    ASSERT_EQ(buf4[0], 4);

    // move, then move back
    buf4 = std::move(buf3 = std::move(buf4));
    ASSERT_EQ(buf4.size(), 1);
    ASSERT_EQ(buf4[0], 4);
    // still alive? good work!
}

// /*
//  * Tests the insert_at_cursor function with an r-value.
//  * Also checks for the efficiency (no copying!)
//  */
TEST(GapBufferTest, TEST7F_INSERT_RVALUE_TIME) {
    GapBuffer<std::vector<int>> buf;
    GapBuffer<std::vector<int>> answer;
    std::vector<int> vec(10000, 1);
    answer.insert_at_cursor(vec);
    auto start = std::chrono::high_resolution_clock::now();
    buf.insert_at_cursor(std::move(vec));
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    ASSERT_TRUE(elapsed.count() < 10) << "inserting an r-value must be instantaneous";
    ASSERT_TRUE(buf == answer); // elements should still be the same
}

// /*
//  * Tests that insert_at_cursor still corrects correctly for an l-value.
//  */
TEST(GapBufferTest, TEST7G_INSERT_LVALUE) {
    GapBuffer<std::vector<int>> buf;
    std::vector<int> vec(10000, 42);
    std::vector<int> answer(10000, 42);
    buf.insert_at_cursor(vec);
    ASSERT_EQ(vec, answer); // vec better still be intact
}

// /*
//  * A stress test for r-value insert_at_cursor.
//  * Calls move constructor, which in turn must move assign
//  * different elements within the nested class.
//  */
TEST(GapBufferTest, TEST7H_NESTED_INSERT) {
    GapBuffer<GapBuffer<int>> buf;
    GapBuffer<int> toAdd{1, 2, 3};
    buf.insert_at_cursor(toAdd);
    GapBuffer<int> buf2(10000, 1);
    buf.insert_at_cursor(std::move(buf2));
    GapBuffer<int> answer{1, 2, 3};
    GapBuffer<int> answer2(10000, 1);

    ASSERT_EQ(buf[0], answer);
    ASSERT_EQ(buf[1], answer2);
    ASSERT_EQ(buf.size(), 2);
}
#endif

#if TEST9_ENABLED
// /*
//  * Verifies basic functionality of emplace
//  */
TEST(GapBufferTest, TEST_EMPLACE_BASIC) {
    GapBuffer<std::vector<int>> buf;
    buf.emplace_at_cursor(10, 3);
    ASSERT_TRUE(buf[0] == std::vector<int>(10, 3));

    GapBuffer<GapBuffer<int>> buf2;
    buf2.emplace_at_cursor(42, 24);
    ASSERT_TRUE(buf2[0] == GapBuffer<int>(42, 24));
}

// /*
//  * Verifies edge case when parameter back is empty.
//  */
TEST(GapBufferTest, TEST9B_EMPLACE_EDGE) {
    GapBuffer<std::string> buf;
    buf.emplace_at_cursor();
    ASSERT_EQ(buf.size(), 1);
    ASSERT_EQ(buf[0], "");
}

// /*
//  * Tests that emplace_at_cursor is much faster than insert_at_cursor
//  * Scenario: which one is faster?
//  *     (1) Create a vector<vector<int>> vec_three = {vec, vec, vec}, then insert into GapBuffer
//  *     (2) Pass (3, vec) to the fill constructor of vector<vector<int>>
//  * Notice that (1) creates vec_three, then insert creates a copy -> 2 nested vectors.
//  * While (2) only creates 1 nested vector.
//  */
TEST(GapBufferTest, TEST9C_EMPLACE_TIME) {
    GapBuffer<std::vector<std::vector<int>>> buf;
    std::vector<int> vec(100000, 3);
    auto start_insert = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 10; ++i) {
        std::vector<std::vector<int>> vec_three = {vec, vec, vec};
        buf.insert_at_cursor(vec_three);
    }
    auto end_insert = std::chrono::high_resolution_clock::now();
    auto elapsed_insert = std::chrono::duration_cast<std::chrono::microseconds>
            (end_insert - start_insert);

    GapBuffer<std::vector<int>> buf2;
    auto start_emplace = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 10; ++i) {
        buf.emplace_at_cursor(3, vec);
    }
    auto end_emplace = std::chrono::high_resolution_clock::now();
    auto elapsed_emplace = std::chrono::duration_cast<std::chrono::microseconds>
            (end_emplace - start_emplace);

    ASSERT_TRUE(elapsed_insert.count() > 3*elapsed_emplace.count()) << "Emplacing should be much faster than inserting";
}
#endif
