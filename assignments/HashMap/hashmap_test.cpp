#include <vector>
#include <unordered_map>

#include "test_settings.h"
#include "gtest/gtest.h"
#include "hashmap.h"

// ----------------------------------------------------------------------------------------------
/* Type Alias and Common Test Utilities */
using clock_type = std::chrono::high_resolution_clock;
using ns = std::chrono::nanoseconds;
const std::vector<std::pair<std::string, int> > vec {
    {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1},
    {"D", 10}, {"K", 30}, {"Avery", 2020}, {"Anna", 2020}, {"Ali", 2019}
};

const std::vector<std::string> keys {"A", "B", "C", "Not found"};

#define CHECK_MAP_EQUAL(map, answer) \
{ \
    ASSERT_TRUE((answer).empty() == (map).empty()); \
    ASSERT_TRUE((answer).size() == (map).size()); \
    for (const auto& [key, mapped] : (answer)) { \
        ASSERT_TRUE((map).contains(key) != false) << "Map doesn't contain " << key << "-" << mapped; \
        ASSERT_TRUE((map).at(key) == mapped); \
    } \
}

// ----------------------------------------------------------------------------------------------
/* Milestone 1 Test Cases */

/*
* Verifies basic operations by comparing behavior with std::unordered_map
*      - default ctor
*      - size, empty, bucket_count
*      - contains, at (used as an r-value)
*
* Mainly checking that CHECK_MAP_EQUAL compiles correctly.
*/
#if RUN_TEST_1A
TEST(HashMapTest, TEST_1A_BASIC) {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    CHECK_MAP_EQUAL(map, answer);
    ASSERT_EQ(map.bucket_count(), 10);
}
#endif

/*
* Verifies functionality of insert.
*/
#if RUN_TEST_1B
TEST(HashMapTest, TEST_1B_INSERT) {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    CHECK_MAP_EQUAL(map, answer);
    ASSERT_EQ(map.bucket_count(), 10);

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
        CHECK_MAP_EQUAL(map, answer);
    }
    ASSERT_EQ(map.bucket_count(), 10);
}
#endif

/*
* Tests clear operations, and ensure map is in valid
* state after a call to clear.
*/
#if RUN_TEST_1C
TEST(HashMapTest, TEST_1C_CLEAR) {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (size_t j = 0; j < 3; ++j) {
       for (const auto& kv_pair : vec) {
           answer.insert(kv_pair);
           map.insert(kv_pair);
       }

       answer.clear();
       map.clear();

       CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

/*
* Tests whether at correctly returns a reference.
*/
#if RUN_TEST_1D
TEST(HashMapTest, TEST_1D_AT) {
    HashMap<std::string, int> map;
    map.insert({"A", 42});
    map.insert({"B", 137});
    map.insert({"C", -1});

    // at returns a reference, so assigning their return value on the left-hand value should
    // change the contents of the map.
    map.at("A") = 42;
    map.at("B") = 137;
    map.at("A") = -42;
    map.at("C") = map.at("A");
    ASSERT_EQ(map.at("A"), -42);
    ASSERT_EQ(map.at("B"), 137);
    ASSERT_EQ(map.at("C"), -42);

    // verify that we can save the reference returned by at.
    auto copy = map.at("A");
    auto& ref = map.at("B");
    copy = 0;
    ref = 0;
    ASSERT_EQ(copy, 0);
    ASSERT_EQ(map.at("A"), -42);
    ASSERT_EQ(map.at("B"), 0);

    // verify that correct exceptions are thrown
    HashMap<std::string, int> map2;
    bool correct_exception = false;
    try {
        map2.at("Exists") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = true;
    }
    ASSERT_TRUE(correct_exception);
    map2.insert({"Exists", 4});
    ASSERT_TRUE(map2.contains("Exists"));
    ASSERT_FALSE(map2.contains("Does not exist"));
    map2.insert({"Does not exist", 1});
    correct_exception = true;
    try {
        map2.at("Does not exist") = 5;
    } catch (const std::out_of_range& e) {
        correct_exception = false;
    }
    ASSERT_TRUE(correct_exception);
}
#endif

template <typename... Ts>
void ensure_at_return_is_const(Ts...) {}

template <typename T>
auto ensure_at_return_is_const(const T& map) -> decltype((void) (map.at(0) = 3), void()) {
    ASSERT_TRUE(false);
    // If you run into an error here, this means your return value of const
    // at is a non-const reference. Why is that wrong?

    // Details about this error (it's a hacky sfinae technique):
    // we tried running the following code segment (in the decltype line)
    //      const T& map;
    //      map.at(0) = 3;
    // if the segment compiles, then this function is run (which fails)
    // if the segment does not compile, the variadic template before runs (which passes)
}

#if RUN_TEST_1E
TEST(HashMapTest, TEST_1E_AT_CONST_CORRECT) {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map1;
    CHECK_MAP_EQUAL(map1, answer); // both maps should be empty

    for (const auto& kv_pair : vec) {
       answer.insert(kv_pair);
       map1.insert(kv_pair);
       CHECK_MAP_EQUAL(map1, answer); // both maps have same elements
    }

    // create const references to the maps
    // to see if these const references work correctly
    const auto& c_ref_answer = answer;
    const auto& c_ref_map1 = map1;

    // check if we can still check contents of maps as equal
    // even when dealing with const references
    CHECK_MAP_EQUAL(c_ref_map1, c_ref_answer);

    for (const auto& [key, mapped] : answer) {
        ASSERT_TRUE(c_ref_map1.contains(key));
        ASSERT_TRUE(map1.contains(key));
        ASSERT_TRUE(c_ref_map1.at(key) == mapped); // confirm parameter is a const reference
        ASSERT_TRUE(map1.at(key) == mapped);
        CHECK_MAP_EQUAL(map1, answer);
    }

    map1.erase("A");
    answer.erase("A");

    for (const auto& [key, mapped] : answer) {
        ASSERT_TRUE(c_ref_map1.contains(key));
        ASSERT_TRUE(c_ref_map1.at(key) == mapped); // confirm parameter is a const reference
        CHECK_MAP_EQUAL(map1, answer);
    }

    HashMap<int, int> map3;
    map3.insert({0, 0});
    ensure_at_return_is_const(map3); // confirm return value is a const reference
}
#endif

/*
* Tests ctor taking in num_buckets, while hash function
* still uses the default. Also tests correct function of load_factor.
*/
#if RUN_TEST_1F
TEST(HashMapTest, TEST_1F_CUSTOM_BUCKET_COUNT) {
    HashMap<int, int> many_buckets(10000);
    HashMap<int, int> one_bucket(1);
    std::unordered_map<int, int> answer;

    for (int i = 0; i < 100; ++i) {
       many_buckets.insert({i, -i});
       one_bucket.insert({i, -i});
       answer.insert({i, -i});
    }

    CHECK_MAP_EQUAL(many_buckets, answer);
    CHECK_MAP_EQUAL(one_bucket, answer);

    ASSERT_EQ(many_buckets.bucket_count(), 10000);
    ASSERT_EQ(one_bucket.bucket_count(), 1);

    float epsilon = 0.001;
    ASSERT_LT(std::abs(many_buckets.load_factor() - 0.01), epsilon);
    ASSERT_LT(std::abs(one_bucket.load_factor() - 100), epsilon);
}
#endif

/*
* Tests ctor taking in a num_buckets and hash function.
* We use a hash function that distributes similar inputs
* more randomly across the buckets.
*/
#if RUN_TEST_1G
TEST(HashMapTest, TEST_1G_CUSTOM_HASH_FUNCTION) {
    using K = int;
    using V = char;
    auto identity_shifted = [](const K& key) {
       return (key * 43037 + 52081) % 79229;
    };

    // hey, didn't you use this from assignment 1?
    // now you know what the priority queue decltype comes from!
    HashMap<K, V, decltype(identity_shifted)> map(75, identity_shifted);
    std::unordered_map<K, V> answer;

    for (int i = 0; i < 50; ++i) {
       map.insert({i, 'a' + i});
       answer.insert({i, 'a' + i});
       CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

/*
* Tests erase operation in combination with basic operations.
*/
#if RUN_TEST_1H
TEST(HashMapTest, TEST_1H_ERASE) {
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        answer.insert(kv_pair);
        map.insert(kv_pair);
    }

    // remove one and insert two back at a time
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[i];
        auto& pair2 = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);
        CHECK_MAP_EQUAL(map, answer);

        answer.erase("Not a key");
        map.erase("Not a key");
        CHECK_MAP_EQUAL(map, answer);

        answer.insert(pair2);
        map.insert(pair2);
    }

    // remove one at a time, until map is empty
    for (size_t i = 0; i < vec.size(); ++i) {
        auto& [key1, mapped1] = vec[(i+3) % vec.size()];
        answer.erase(key1);
        map.erase(key1);

        CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

/*
* Verifies external correct after call to M1_Review.
* Note that this does not actually verify if the linked lists are correct,
* merely verifies that after call to M1_Review, all method calls are still valid,
* and bucket_count is correct.
*/
#if RUN_TEST_1I
TEST(HashMapTest, TEST_1I_REHASH_BASIC) {

    HashMap<int, int> map;
    std::unordered_map<int, int> answer;
    std::vector<int> vals;

    ASSERT_TRUE(map.bucket_count() == 10);
    for (size_t M1_Review_count : {10, 20, 20, 15, 1000, 100, 2, 1, 2, 1}) {

       map.rehash(M1_Review_count);

       ASSERT_TRUE(map.bucket_count() == M1_Review_count);

        for (size_t i = 0; i < 18; ++i) {
            CHECK_MAP_EQUAL(map, answer);
            map.erase(i);
            answer.erase(i);
        }
        map.clear();
        answer.clear();
        CHECK_MAP_EQUAL(map, answer);

        for (size_t i = 1; i < 17; ++i) {
            map.insert({i, i});
            answer.insert({i, i});
        }
    }

    try {
        map.rehash(0);
        ASSERT_TRUE(false); // you didn't throw an exception!
    } catch (const std::out_of_range& e) {
        // Great! You threw the correct exception.
    } catch (const std::exception& e) {
        ASSERT_TRUE(false); // you threw the wrong exception!
    }
}
#endif

// __attribute__((unused)) void unused_rehash_correct_by_time() {
//    /*
//     * This was previously a test when rehash was part of the assignment. However, it did not work
//     * well with the compiler optimizations, so we will not include this test. If a future lecturer
//     * wants to add rehash back into the assignment, they can experiment with this test.
//     *
//     * Here's to spending lots of time on tests that get removed from the assignment!
//     *
//     * This test tries to check if you hashed elements to the buckets correctly
//     * by adding a specific number of elements, and measuring the time it takes
//     * to call contains. The idea is that if bucket 0 is supposed to have 10 times
//     * the number of elements as bucket 1, calling contains on an element
//     * that should be hashed to bucket 0 but is not present should take 10 times
//     * longer that calling contains on an element that should be hashed to bucket 1
//     * but is not present.
//     *
//     * Obviously, this is less than perfect, since it's hard to predict how
//     * fast your computer is running linked list operations. There are two parameters
//     * below. One is called tolerance (let me know if I'm using the term incorrectly)
//     * which determines the amount of leeway we give for any differences from the
//     * expected time (0.5 means 50% leeway). The second is trials. You can try
//     * increasing that to see if you get better results.
//     *
//     * Play around with this and let me know if you find something interesting,
//     * or if you have suggestions to make this test better. There are also
//     * cool number theory results, as hashing is inherently an idea from
//     * cryptography. Ask Avery if you are curious!
//     */
//     using K = int;
//     using V = int;
//     float tolerance = 0.5;      // makes me feel like an engineer, probably using the term wrong
//     int trials = 1000;           // increase if necessary, Central Limit Theorem!

//     // in case std::hash<K> is different on your compiler.
//     auto identity = [](const K& key) { return key; };

//     HashMap<K, V, decltype(identity)> map(6, identity);

//     // bucket(elements): 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
//     for (int i = 1; i <= 100; ++i) map.insert({6*i+1, i}); // bucket 1
//     for (int i = 1; i <= 1500; ++i) map.insert({6*i+2, i}); // bucket 2
//     for (int i = 1; i <= 500; ++i) map.insert({6*i+3, i}); // bucket 3
//     for (int i = 1; i <= 1500; ++i) map.insert({6*i+4, i}); // bucket 4
//     for (int i = 1; i <= 6000; ++i) map.insert({6*i+5, i}); // bucket 5
//     std::unordered_map<int, float> bucket_times_6;

//     for (int i = 0; i < 6; ++i) {
//         auto start = clock_type::now();
//         for (int j = 0; j < trials; ++j) map.contains(i);
//         auto end = clock_type::now();
//         auto elapsed = std::chrono::duration_cast<ns>(end - start);
//         bucket_times_6.insert({i, elapsed.count()});
//     }

//     map.rehash(3);
//     std::unordered_map<int, float> bucket_times_3;
//     for (int i = 0; i < 3; ++i) {
//         auto start = clock_type::now();
//         for (int j = 0; j < trials; ++j) map.contains(i);
//         auto end = clock_type::now();
//         auto elapsed = std::chrono::duration_cast<ns>(end - start);
//         bucket_times_3.insert({i, elapsed.count()});
//     }

//     map.rehash(2);
//     std::unordered_map<int, float> bucket_times_2;
//     for (int i = 0; i < 2; ++i) {
//         auto start = clock_type::now();
//         for (int j = 0; j < trials; ++j) map.contains(i);
//         auto end = clock_type::now();
//         auto elapsed = std::chrono::duration_cast<ns>(end - start);
//         bucket_times_2.insert({i, elapsed.count()});
//     }

//     // Time to pull out the Chinese Remainder Theorem!
//     // take the unique bijection Z/6 -> Z/3 x Z/2
//     // bucket(elements) mod 6: 0(0), 1(100), 2(1500), 3(500), 4(1500), 5(6000)
//     // bucket(elements) mod 3: 0+3(500), 1+4(1600), 2+5(7500)
//     // bucket(elements) mod 2: 0+2+4(3000), 1+3+5(7500)

//     float ratio6_10 = float(bucket_times_6[1])/(bucket_times_6[0]+1);
//     float ratio6_23 = bucket_times_6[2]/bucket_times_6[3]; // expected: 1500/500
//     float ratio6_54 = bucket_times_6[5]/bucket_times_6[4]; // expected: 6000/1500
//     float ratio3_10 = bucket_times_3[1]/bucket_times_3[0]; // expected: 1600/500
//     float ratio3_21 = bucket_times_3[2]/bucket_times_3[1]; // expected: 7500/1600
//     float ratio2_10 = bucket_times_2[1]/bucket_times_2[0]; // expected: 7500/3000

//     // experiments are noisy, so let's give you some acceptable tolerance
//     ASSERT_TRUE(ratio6_10 > 10);
//     ASSERT_TRUE(ratio6_23 < (1+tolerance)*1500/500 && ratio6_23 > 1/(1+tolerance)*1500/500);
//     ASSERT_TRUE(ratio6_54 < (1+tolerance)*6000/1500 && ratio6_54 > 1/(1+tolerance)*6000/1500);
//     ASSERT_TRUE(ratio3_10 < (1+tolerance)*1600/500 && ratio3_10 > 1/(1+tolerance)*1600/500);
//     ASSERT_TRUE(ratio3_21 < (1+tolerance)*7500/1600 && ratio3_21 > 1/(1+tolerance)*7500/1600);
//     ASSERT_TRUE(ratio2_10 < (1+tolerance)*7500/3000 && ratio2_10 > 1/(1+tolerance)*7500/3000);

//     // fun fact: we had to add an -O0 flag because the compiler was optimizing our code
//     // a little too well. Turns out that the runtime all of these is the same with optimization (!)
// }


/* Tests whether iterators are supported via a simple for-each loop
* Uses begin() and end() of your HashMap, in addition to
* the ctor and operators =, !=, and ++ of your iterator.
*/
#if RUN_TEST_1J
TEST(HashMapTest, TEST_1J_ITER_FOREACH_ONE_BUCKET) {
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {30, 30}, {140, 140}, {21, 21}
    };

    HashMap<int, int> map10(1); // can your iterator traverse normal use case?
    for (const auto& pair : questions) {
        map10.insert(pair);
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) {
        ASSERT_TRUE(answers10.insert(pair).second == true);
    }

    ASSERT_TRUE(questions == answers10);
}
#endif

/* Tests whether iterators are supported via a simple for-each loop
* Uses begin() and end() of your HashMap, in addition to
* the ctor and operators =, !=, and ++ of your iterator.
*/
#if RUN_TEST_1K
TEST(HashMapTest, TEST_1K_ITER_FOREACH_FILLED_BUCKETS) {

    std::set<std::pair<int, int> > questions;
    auto identity = [](auto i) {return i;};
    HashMap<int, int, decltype(identity)> map10(6, identity); // can your iterator traverse normal use case?
    for (int i = 0; i < 10; ++i) {
        map10.insert({i, i});
        questions.insert({i, i});
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) ASSERT_TRUE(answers10.insert(pair).second == true);
    ASSERT_TRUE(questions == answers10);
}
#endif

/* Tests whether iterators are supported via a simple for-each loop
* Uses begin() and end() of your HashMap, in addition to
* the ctor and operators =, !=, and ++ of your iterator.
*/

#if RUN_TEST_1L
TEST(HashMapTest, TEST_1L_ITER_FOREACH_SPLIT_BUCKETS) {
    std::set<std::pair<int, int> > questions;
    auto identity = [](auto i) {return i;};
    HashMap<int, int, decltype(identity)> map10(6, identity);            // can your iterator traverse normal use case?
    for (int i = 0; i < 20; i += 2) {
        map10.insert({i, i});
        questions.insert({i, i});
    }
    std::set<std::pair<int, int> > answers10;
    for (const auto& pair : map10) ASSERT_TRUE(answers10.insert(pair).second == true);
    ASSERT_TRUE(questions == answers10);
}
#endif

/* Tests a few edge cases for your iterator, such as checking the bounds */
#if RUN_TEST_1M
TEST(HashMapTest, TEST_1M_ITER_FOREACH_EDGE) {
    std::set<std::pair<int, int> > questions {
        {6, 6}, {1, 1}, {2, 2}, {3, 3}, {4, 4}
    };

    HashMap<int, int> map1(1);      // one bucket with all the elements
    HashMap<int, int> map5(5);      // exactly one per bucket
    HashMap<int, int> map10(100);   // a lot of empty buckets
    HashMap<int, int> empty;        // won't add anything to this one

    for (const auto& pair : questions) {
        map1.insert(pair);
        map5.insert(pair);
        map10.insert(pair);
    }
    std::set<std::pair<int, int> > answers1, answers5, answers10;
    for (const auto& pair : map1) ASSERT_TRUE(answers1.insert(pair).second == true);
    for (const auto& pair : map5) ASSERT_TRUE(answers5.insert(pair).second == true);
    for (const auto& pair : map10) ASSERT_TRUE(answers10.insert(pair).second == true);
    for (const auto& pair : empty) ASSERT_TRUE(false); // this should not run!

    ASSERT_TRUE(questions == answers1);
    ASSERT_TRUE(questions == answers5);
    ASSERT_TRUE(questions == answers10);
}
#endif

/* Tests the more advanced operators, that are required
 * since the iterator can be a forward iterator */
#if RUN_TEST_1N
TEST(HashMapTest, TEST_1N_ITER_FORWARD_OPERATORS) {
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 4}, {3, 9}
    };
    // Note to reader: yes, I spent so much time writing these awesome test cases
    // and then decided to make this part optional. What a great use of my spring break.
    // It's not like I have anything else to do, right?


    HashMap<int, int> map;            // can your iterator traverse normal use case?
    for (const auto& pair : questions) {
        map.insert(pair);
    }

    // note: we make no assumptions about which specifc element is in which order!
    auto iter = map.begin();                    // iter->0th element
    ASSERT_TRUE((*iter).first == (*iter).second);   // behavior of * operator
    ASSERT_TRUE(iter-> first * iter->first == iter->second);// behavior of -> operator
    ASSERT_TRUE(iter == iter);                      // behavior of == operator
    ASSERT_FALSE(iter != iter);                   // behavior of != operator

    (*iter).second = -1;                        // behavior of * operator as an l-value
    ASSERT_TRUE((*iter).second == -1);    // behavior of * operator as an r-value
    iter->second = -2;                          // behavior of -> operator as an l-value
    ASSERT_TRUE(iter->second == -2);      // behavior of -> operator as an r-value

    // verify correct prefix/postfix behavior (this was very tedious)
    HashMap<int, int>::iterator iter0 = iter; // just to prove why type aliases are helpful
    auto iter1 = ++iter;                      // though auto usually works as well
    auto iter2 = ++iter;
    auto iter3 = ++iter;
    // iter0 = {1, -2}, iter1 = {2, 4}, iter3 = {3, 9}
    ASSERT_TRUE(iter0->first + iter1->first + iter2->first == 6);
    ASSERT_TRUE(iter0->first * iter1->first * iter2->first == 6);
    ASSERT_TRUE(iter0->second + iter1->second + iter2->second == 11);
    ASSERT_TRUE(iter0->second * iter1->second * iter2->second == -72);
    ASSERT_TRUE(iter == map.end());
    iter = iter0;                       // iter->0
    auto& iter_ref = ++iter;            // iter/iter_ref->1
    ASSERT_TRUE(iter_ref == iter1);
    auto iter_ref_copy = ++iter_ref;    // iter_ref_copy->2, iter/iter_ref->2
    ASSERT_TRUE(iter_ref_copy == iter2);
    ASSERT_TRUE(iter_ref == iter2);

    auto iter_post = iter++;            // iter/iter_ref->3, iter_post->2
    ASSERT_TRUE(iter_post == iter2);
    ASSERT_TRUE(iter == iter3);
    iter_ref = map.begin();             // iter/iter_ref->0
    ASSERT_TRUE(iter == iter0);

    // Big LOL - see if you can actually trace the ++++++ operator.
    auto iter_chain_pre = ++++++iter;   // iter_chain_pre->3, iter/iter_ref->3
    ASSERT_TRUE(iter == iter3);
    ASSERT_TRUE(iter_chain_pre == iter3);
    iter_ref = map.begin();             // iter/iter_ref->0
    auto iter_chain_post = iter++++++;  // iter/iter_ref->1, iter_chain_post->0
    ASSERT_TRUE(iter == iter1);
    ASSERT_TRUE(iter_chain_post == iter0);
    // presumably if you pass the above ones, you probably have it working
    // so I'm not gonna think about what ++iter++++ would be
}
#endif

/* Stress test of the basic operators of iterators using STL copy */
#if RUN_TEST_1O
TEST(HashMapTest, TEST_1O_ITER_ALGORITHM_COPY) {
    HashMap<int, int> map;
    std::unordered_map<int, int> answer;

    for (int i = -17; i < 10; ++i) {
        map.insert({i, i*i});
    }

    std::copy(map.begin(), map.end(), std::inserter(answer, answer.begin()));
    CHECK_MAP_EQUAL(map, answer);
    answer.clear();

    const auto& c_map = map;
    std::copy(c_map.begin(), c_map.end(), std::inserter(answer, answer.begin()));
    CHECK_MAP_EQUAL(map, answer);
}
#endif

/* Stress test of the multipass functionality of forward iterators using STL is_permutation */
#if RUN_TEST_1P
TEST(HashMapTest, TEST_1P_ITER_ALGORITHM_PERMUTATION) {
    /* Stress test of the multipass functionality of forward iterators using STL is_permutation */
    auto identity_shifted = [](const int& key) {
       return (key * 43037 + 52081) % 79229;
    };

    HashMap<int, int, decltype(identity_shifted)> map1(1, identity_shifted);
    HashMap<int, int> map2(3);
    std::unordered_map<int, int> answer;

    for (int i = -100; i < 100; ++i) {
        map1.insert({i, i*i});
        map2.insert({i, i*i});
        answer.insert({i, i*i});
    }

    ASSERT_TRUE(std::is_permutation(map1.begin(), map1.end(), answer.begin(), answer.end()));
    ASSERT_TRUE(std::is_permutation(answer.begin(), answer.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(std::is_permutation(map2.begin(), map2.end(), answer.begin(), answer.end()));
    ASSERT_TRUE(std::is_permutation(answer.begin(), answer.end(), map2.begin(), map2.end()));
    ASSERT_TRUE(std::is_permutation(map2.begin(), map2.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(std::is_permutation(map1.begin(), map1.end(), map2.begin(), map2.end()));
    ASSERT_TRUE(std::is_permutation(map1.begin(), map1.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(std::is_permutation(map2.begin(), map2.end(), map2.begin(), map2.end()));

    map1.insert({-1000, 1000});
    map1.rehash(4);
    map2.rehash(5);
    ASSERT_TRUE(!std::is_permutation(map1.begin(), map1.end(), answer.begin(), answer.end()));
    ASSERT_TRUE(!std::is_permutation(answer.begin(), answer.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(!std::is_permutation(map2.begin(), map2.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(!std::is_permutation(map1.begin(), map1.end(), map2.begin(), map2.end()));
    ASSERT_TRUE(std::is_permutation(map1.begin(), map1.end(), map1.begin(), map1.end()));
    ASSERT_TRUE(std::is_permutation(map2.begin(), map2.end(), map2.begin(), map2.end()));
}
#endif


/* Tests the const-correct of your iterator class by asking for const_iterators via begin() and end() */
#if RUN_TEST_1Q
TEST(HashMapTest, TEST_1Q_ITER_CONST_CORRECT) {
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    /* testing const_iterator (iterator to const std::pair) */
    HashMap<int, int> map;
    for (const auto& pair : questions) map.insert(pair);
    const auto& const_map = map;
    std::set<std::pair<int, int> > answers;
    for (const auto& pair : const_map) ASSERT_TRUE(answers.insert(pair).second == true);
    ASSERT_TRUE(questions == answers);

    HashMap<int, int>::const_iterator iter = const_map.begin();

    ASSERT_TRUE((*iter).first == (*iter).second);   // behavior of * operator
    ASSERT_TRUE(iter->first == iter->second);       // behavior of -> operator
    ASSERT_TRUE(iter == iter);                      // behavior of == operator
    ASSERT_TRUE(!(iter != iter));                   // behavior of != operator

    ASSERT_TRUE(iter->second == (*iter).second);
    auto iter1 = ++iter;
    auto iter2 = ++iter;
    auto iter3 = iter++;
    ASSERT_TRUE(iter == const_map.end());
    ASSERT_TRUE(iter3 == iter2);
    ASSERT_TRUE(iter1 != iter2);

    /* We could have the entire operator tests here, though that feels unnecessary */
}
#endif

/* Test the distinction between const iterator and const_iterator */
#if RUN_TEST_1R
TEST(HashMapTest, TEST_1R_ITER_CONST_CORRECT) {
    /* Test the distinction between const iterator and const_iterator */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    HashMap<int, int> map;
    for (const auto& pair : questions) map.insert(pair);

    /* test behavior of const iterator */
    HashMap<int, int>::iterator iter = map.begin();
    const HashMap<int, int>::iterator c_iter = map.begin();
    const HashMap<int, int>::iterator& copy = iter;
    const HashMap<int, int>::iterator& copy_next = ++iter;

    ASSERT_TRUE(map.begin() == c_iter);
    ASSERT_TRUE(copy == iter);
    ASSERT_TRUE(copy_next == iter);
    ASSERT_TRUE(c_iter != iter);

    // the iterator is const, but the stuff the iterator points to is not const.
    (*c_iter).second = -1;                                   // behavior of * operator as an l-value
    ASSERT_TRUE((*c_iter).second == -1);              // behavior of * operator as an r-value
    c_iter->second = -2;                                     // behavior of -> operator as an l-value
    ASSERT_TRUE(c_iter->second == -2);                // behavior of -> operator as an r-value

    // these should not compile:
    // *iter = {0, 0};  // *iter is a std::pair<const K, M>, since K is const, = is deleted
    // ++c_iter;        // ++ is non-const

    ASSERT_TRUE(++++iter == map.end());

    /* test behavior of const const_iterator */
    const auto& const_map = map;
    HashMap<int, int>::const_iterator const_iter = const_map.begin();
    const HashMap<int, int>::const_iterator c_const_iter_next = ++const_map.begin();
    const HashMap<int, int>::const_iterator c_const_iter = const_map.begin();

    // the key here is that these should compile.
    ++const_iter;
    ASSERT_TRUE((*c_const_iter).second == -2);
    ASSERT_TRUE(c_const_iter->second == -2);
    ASSERT_TRUE(const_iter == c_const_iter_next);
    ASSERT_TRUE(c_const_iter == const_map.begin());

    // these should not compile:
    // ++c_const_iter;
    // c_const_iter->second = 2;
    // const_iter->second = 2;

    // TODO: create SFINAE tests for testing iterator code that should not compile
}
#endif

#if RUN_TEST_1S
TEST(HashMapTest, TEST_1S_ITER_CONST_CONVERSION) {
    /* Tests whether an iterator can implicitly convert to a const_iterator */
    std::set<std::pair<int, int> > questions {
        {1, 1}, {2, 2}, {3, 3}
    };

    HashMap<int, int> map;
    const auto& cmap = map;
    for (const auto& pair : questions) map.insert(pair);

    HashMap<int, int>::const_iterator c_iter = cmap.begin();
    HashMap<int, int>::iterator iter = map.begin();
    HashMap<int, int>::const_iterator converted_iter = iter; // this results in an implicit conversion to const_iterator
    ASSERT_TRUE(converted_iter == c_iter);
    ++c_iter;
    ++iter;
    ++converted_iter;

    ASSERT_TRUE((static_cast<HashMap<int, int>::const_iterator>(iter) == c_iter));
    ASSERT_TRUE((static_cast<HashMap<int, int>::const_iterator>(iter) == converted_iter));

    ++iter;
    ++iter;
    HashMap<int, int>::const_iterator converted_end = iter;
    ASSERT_TRUE(iter == map.end());
    ASSERT_TRUE(converted_end == cmap.end());
}
#endif

#if RUN_TEST_1T
TEST(HashMapTest, TEST_1T_ITER_FIND_MEMBER) {
    /*
    * Verifies functionality of the find member function, which uses the iterator class.
    */
    HashMap<std::string, int> map;

    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
    }

    {
        // test the non-const version of HashMap::find
        auto iter_avery = map.find("Avery");
        ASSERT_TRUE(iter_avery->first == "Avery");
        ASSERT_TRUE(iter_avery->second == 2020);

        auto iter_ali = map.find("Ali");
        ASSERT_TRUE(iter_ali->first == "Ali");
        ASSERT_TRUE(iter_ali->second == 2019);

        iter_ali->second = 2018;
        auto iter_repeat = map.find("Ali");
        ASSERT_TRUE(iter_repeat->first == "Ali");
        ASSERT_TRUE(iter_repeat->second == 2018);
        ASSERT_TRUE(iter_ali == iter_repeat);
        ASSERT_TRUE(iter_avery != iter_ali);
        iter_ali->second = 2019;

        auto iter_not_found = map.find("Not found");
        ASSERT_TRUE(iter_not_found == map.end());
    }

    {
         // test the const version of HashMap::find
        const auto& cmap = map;
        auto iter_avery = cmap.find("Avery");
        ASSERT_TRUE(iter_avery->first == "Avery");
        ASSERT_TRUE(iter_avery->second == 2020);

        auto iter_ali = cmap.find("Ali");
        ASSERT_TRUE(iter_ali->first == "Ali");
        ASSERT_TRUE(iter_ali->second == 2019);
        auto iter_repeat = cmap.find("Ali");
        ASSERT_TRUE(iter_ali == iter_repeat);
        ASSERT_TRUE(iter_avery != iter_ali);

        auto iter_not_found = cmap.find("Not found");
        ASSERT_TRUE(iter_not_found == cmap.end());
    }
}
#endif

#if RUN_TEST_1U
TEST(HashMapTest, TEST_1U_ITER_INSERT_MEMBER) {
    /*
    * Verifies functionality of the insert member function, returns an iterator.
    * The previous test B ignores the return value.
    */
    HashMap<std::string, int> map;

    auto [iter_avery0, added0] = map.insert({"Avery", 3});
    ASSERT_TRUE(added0);
    ASSERT_TRUE(iter_avery0 == map.begin());
    ASSERT_TRUE(iter_avery0->first == "Avery");
    ASSERT_TRUE(iter_avery0->second == 3);

    auto [iter_avery1, added1] = map.insert({"Avery", 3});
    ASSERT_FALSE(added1);
    ASSERT_TRUE(iter_avery0 == iter_avery1);
    iter_avery1->second = 4;
    ASSERT_TRUE(iter_avery0->second == 4);

    auto [iter_anna2, added2] = map.insert({"Anna", 2});
    ASSERT_TRUE(added2);
    ASSERT_TRUE(iter_anna2->first == "Anna");
    ASSERT_TRUE(iter_anna2->second == 2);
}
#endif

#if RUN_TEST_1V
TEST(HashMapTest, TEST_1V_ITER_ERASE_MEMBER) {
    /*
    * Verifies functionality of the erase and insert. Note that this erase test takes in a parameter
    * that is a const_iterator, different from test G which only takes in a key.
    */
    HashMap<std::string, int> map;

    map.insert({"Avery", 3});
    auto iter = map.erase(map.begin());
    ASSERT_TRUE(iter == map.end());

    auto [iter_avery0, added0] = map.insert({"Avery", 3});
    ASSERT_TRUE(added0);
    ASSERT_TRUE(iter_avery0 == map.begin());
    ASSERT_TRUE(iter_avery0->first == "Avery");
    ASSERT_TRUE(iter_avery0->second == 3);

    auto result = map.erase(iter_avery0);
    ASSERT_TRUE(result == map.end());
    ASSERT_TRUE(map.empty());

    auto [iter_avery1, added1] = map.insert({"Avery", 4});
    ASSERT_TRUE(added0);
    ASSERT_TRUE(iter_avery1->second == 4);

    auto [iter_anna2, added2] = map.insert({"Anna", 2});
    ASSERT_TRUE(added2);
    ASSERT_TRUE(iter_anna2->first == "Anna");
    ASSERT_TRUE(iter_anna2->second == 2);

    auto next = map.erase(iter_anna2);
    ASSERT_TRUE(next == map.end() || next == iter_avery1);
    ASSERT_TRUE(iter_avery1 == map.begin());
    auto next_avery = map.erase(iter_avery1);
    ASSERT_TRUE(next_avery == map.end());
    ASSERT_TRUE(map.begin() == map.end());
}
#endif

#if RUN_TEST_2A
TEST(HashMapTest, TEST_2A_RANGE_CTOR_BASIC) {
    /* Simple test of the range ctor taking in two iterators to another collection */
    std::vector<std::pair<std::string, int>> values {
        {"Ignore me", 100}, {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };
    std::unordered_map<std::string, int> answer {values.begin()++, values.end()}; // skips the first "Ignore me" pair
    HashMap<std::string, int> map {values.begin()++, values.end()};

    CHECK_MAP_EQUAL(map, answer);
}
#endif

#if RUN_TEST_2B
TEST(HashMapTest, TEST_2B_RANGE_CTOR_EDGE) {
    /* Simple test of the range ctor taking in two iterators to another collection */
    std::vector<std::pair<std::string, int>> values {
        {"Ignore me", 100}
    };
    {
        // range has exactly one element
        std::unordered_map<std::string, int> answer {values.begin(), values.end()};
        HashMap<std::string, int> map {values.begin(), values.end()};
        CHECK_MAP_EQUAL(map, answer);
    }
    {
        // range has no elements (first == last) because two ends were passed in
        std::unordered_map<std::string, int> answer {values.end(), values.end()};
        HashMap<std::string, int> map {values.end(), values.end()};
        CHECK_MAP_EQUAL(map, answer);
    }
    {
        // range has no elements (first == last) since the container was empty
        std::vector<std::pair<std::string, int>> empty;
        std::unordered_map<std::string, int> answer {empty.begin(), empty.end()};
        HashMap<std::string, int> map {empty.begin(), empty.end()};
        CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

#if RUN_TEST_2C
TEST(HashMapTest, TEST_2C_INITIALIZER_LIST_CTOR_BASIC) {
    /* Tests initializer_list via a simple example */
    std::unordered_map<std::string, int> answer {
        {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };

    HashMap<std::string, int> map {
        {"A", 3}, {"B", 2}, {"C", 1}, {"A", -5}, {"B", 3}, {"A", 5}, {"C", 1}
    };

    CHECK_MAP_EQUAL(map, answer);
}
#endif

#if RUN_TEST_2D
TEST(HashMapTest, TEST_2D_INITIALIZER_LIST_CTOR_EDGE) {
    /* Tests initializer_list via a few edge cases example */
    std::unordered_map<std::string, int> answer {{"A", 3}};
    HashMap<std::string, int> map {{"A", 3}};
    CHECK_MAP_EQUAL(map, answer);

    std::unordered_map<std::string, int> empty_answer ({});
    HashMap<std::string, int>  empty ({});
    CHECK_MAP_EQUAL(empty, empty_answer);
}
#endif

#if RUN_TEST_3A
TEST(HashMapTest, TEST_3A_INDEX_OPERATOR) {
    /*
     * Tests the indexing operator to ensure it has the functionality of at(),
     * and also supports auto-insertion.
     */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map;
    CHECK_MAP_EQUAL(map, answer);
    ASSERT_TRUE(answer["Autoinsert"] == map["Autoinsert"]);
    ASSERT_TRUE(answer["Autoinsert"] == int());
    for (const auto& [key, value] : vec) {
       answer[key] = value;
       map[key] = value;
       CHECK_MAP_EQUAL(map, answer);
       ASSERT_TRUE(answer["Autoinsert"] == map["Autoinsert"]);
    }
}
#endif

#if RUN_TEST_3B
template <typename... Ts>
void ensure_no_const_index_op(Ts...) {}

template <typename T>
auto ensure_no_const_index_op(const T& map) -> decltype((void) (map[0]), void()) {
    ASSERT_TRUE(false);
    // The idea of this test is that there are two overloads for ensure_no_const_index_op.
    // The compiler tries this current overload, which only compiles
    // if map[0] compiles, but it is not supposed to
    // because map a const reference and map[0] may autoinsert.

    // If map[0] does not compile (as expected), then
    // the varadic template overload above is called, and no runtime assertion happens.

    // It's tricky to test "this code should not compile" - this is a hacky way to check that.
}

TEST(HashMapTest, TEST_3B_INDEX_CONST_CORRECT) {
    HashMap<int, int> map;
    ensure_no_const_index_op(map);

    // check for parameter const correct
    const int key = 3;
    map[key] = 6;
    int r_key = 5;
    map[r_key] = 2;
    map[r_key] = key;
}
#endif

#if RUN_TEST_3C
TEST(HashMapTest, TEST_3C_STREAM_INSERT_OPERATOR) {
    /*
     * Tries to insert the map into various streams.
     * The format is described on the handout:
     * {Avery:2, Anna:3}
     * {}
     * {Avery:2}
     */
    HashMap<std::string, int> map(1);

    // Test 1: print empty map
    std::ostringstream oss1;
    oss1 << map;
    ASSERT_TRUE(oss1.str() == "{}");

    // Test 2: print map with a single element
    std::ostringstream oss2;
    map.insert({"Anna", 2});
    oss2 << map;
    ASSERT_TRUE(oss2.str() == "{Anna:2}") << oss2.str();

    // Test 3: print map with two elements
    std::ostringstream oss3;
    map.insert({"Avery", 3});
    oss3 << map;
    ASSERT_TRUE(oss3.str() == "{Avery:3, Anna:2}" || oss3.str() == "{Anna:2, Avery:3}");
    auto s = oss3.str();

    // Test 4: print map after a call to erase
    std::ostringstream oss4;
    map.erase("Anna");
    map.at("Avery") = 4;
    oss4 << map;
    ASSERT_TRUE(oss4.str() == "{Avery:4}");

    // Test 5: print map that is key/value pairs reversed
    std::ostringstream oss5;
    HashMap<int, std::string> reverse;
    reverse.insert({3, "Avery"});
    reverse.insert({2, "Anna"});
    oss5 << reverse;
    ASSERT_TRUE(oss5.str() == "{3:Avery, 2:Anna}" || oss5.str() == "{2:Anna, 3:Avery}");

    // Test 6: check ability to chain printing HashMaps to streams.
    std::ostringstream oss;
    oss << "A" << map << 3 << reverse << '\n' << map << map << '\n';
    ASSERT_TRUE(oss.str() == "A{Avery:4}3{3:Avery, 2:Anna}\n{Avery:4}{Avery:4}\n" ||
                oss.str() == "A{Avery:4}3{2:Anna, 3:Avery}\n{Avery:4}{Avery:4}\n");

    // Test 7: stress test to see if the correct elements are being printed
    // Uses fixed 3-digit numbers as the key and values, then prints to stream
    // and checks whether output stream has correct length.
    HashMap<int, int> large;
    for (int i = 100; i <= 999; i++) {
        large.insert({i, i});
    }
    // 900 elements of 3-digit numbers
    // 3 x 900 = 2700 characters for keys
    // 3 x 900 = 2700 characters for values
    // 1 x 900 = 900 characters for colons
    // 2 x 899 = 1798 characters for comma and space between each key/value pair
    // 2 x 1 = 2 characters for beginning and end paranthesis

    for (int num_buckets : {1, 3, 7, 10, 100, 10000}) {
        large.rehash(num_buckets);
        std::ostringstream oss7;
        oss7 << large;
        ASSERT_TRUE(oss7.str().size() == 2700 + 2700 + 900 + 1798 + 2);
    }
}
#endif

#if RUN_TEST_3D
TEST(HashMapTest, TEST_3D_STREAM_INSERT_CONST_CORRECT) {
    /*
     * The exact same as the previous test, except the printing
     * happens through const references to non-const streams.
     */
    HashMap<std::string, int> map(1);
    const auto& cmap = map;

    // Test 1: print empty map
    std::ostringstream oss1;
    oss1 << cmap;
    ASSERT_TRUE(oss1.str() == "{}");

    // Test 2: print map with a single element
    std::ostringstream oss2;
    map.insert({"Anna", 2});
    oss2 << cmap;
    ASSERT_TRUE(oss2.str() == "{Anna:2}");

    // Test 3: print map with two elements
    std::ostringstream oss3;
    map.insert({"Avery", 3});
    oss3 << cmap;
    ASSERT_TRUE(oss3.str() == "{Avery:3, Anna:2}" || oss3.str() == "{Anna:2, Avery:3}");
    auto s = oss3.str();

    // Test 4: print map after a call to erase
    std::ostringstream oss4;
    map.erase("Anna");
    map.at("Avery") = 4;
    oss4 << cmap;
    ASSERT_TRUE(oss4.str() == "{Avery:4}");

    // Test 5: print map that is key/value pairs reversed
    std::ostringstream oss5;
    HashMap<int, std::string> reverse;
    const auto& creverse = reverse;
    reverse.insert({3, "Avery"});
    reverse.insert({2, "Anna"});
    oss5 << creverse;
    ASSERT_TRUE(oss5.str() == "{3:Avery, 2:Anna}" || oss5.str() == "{2:Anna, 3:Avery}");

    // Test 6: check ability to chain printing HashMaps to streams.
    std::ostringstream oss;
    oss << "A" << cmap << 3 << creverse << '\n' << cmap << cmap << '\n';
    ASSERT_TRUE(oss.str() == "A{Avery:4}3{3:Avery, 2:Anna}\n{Avery:4}{Avery:4}\n" ||
                oss.str() == "A{Avery:4}3{2:Anna, 3:Avery}\n{Avery:4}{Avery:4}\n");

    // Test 7: stress test to see if the correct elements are being printed
    // Uses fixed 3-digit numbers as the key and values, then prints to stream
    // and checks whether output stream has correct length.
    HashMap<int, int> large;
    for (int i = 100; i <= 999; i++) {
        large.insert({i, i});
    }
    // 900 elements of 3-digit numbers
    // 3 x 900 = 2700 characters for keys
    // 3 x 900 = 2700 characters for values
    // 1 x 900 = 900 characters for colons
    // 2 x 899 = 1798 characters for comma and space between each key/value pair
    // 2 x 1 = 2 characters for beginning and end paranthesis

    const auto& clarge = large;
    for (int num_buckets : {1, 3, 7, 10, 100, 10000}) {
        large.rehash(num_buckets);
        std::ostringstream oss7;
        oss7 << clarge;
        ASSERT_TRUE(oss7.str().size() == 2700 + 2700 + 900 + 1798 + 2);
    }
}
#endif

#if RUN_TEST_3E
TEST(HashMapTest, TEST_3E_EQUALITY_OPERATOR) {
    /* Checks functionality of == and != */
    HashMap<int, int> map1(100);
    HashMap<int, int> map2(1);
    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));

    // at this point we're assuming that all your operations
    // already work, so we're just testing whether == is correct.

    // Insert exact same elements
    for (int i = -1; i < 100; ++i) {
        map1.insert({i, i*i});
        map2.insert({i, i*i});
        ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
        ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));
    }

    // Change the two maps' elements in a different order.
    // This means the the maps won't be equal until the very end
    for (int i = 0; i < 99; ++i) {
        map1.at(i) = -i*i;
        map2.at(99-i) = -(99-i)*(99-i);
        map1.rehash(i+1);
        map2.rehash(150+i);
        ASSERT_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2);
        ASSERT_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2));
    }
    map1.at(99) = -99*99;
    map2.at(0) = 0;
    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));

    // Try that same thing again, but insert a ton of rehash calls between them.
    for (int i = 0; i < 99; ++i) {
        map1.erase(i);
        map2.erase(99-i);
        map1.rehash(i+150);
        map2.rehash(1+i);
        ASSERT_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2);
        ASSERT_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2));
    }
    map1.erase(99);
    map2.erase(0);
    map1.rehash(1);
    map2.rehash(1);
    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));
    // consistency after a call to clear
    map1.clear();
    map2.clear();
    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));
}
#endif

#if RUN_TEST_3F
TEST(HashMapTest, TEST_3F_EQUALITY_CONST_CORRECT) {
    /*
     * Checks that your HashMap class is const-correct.
     * The hard part about this test is not getting it to pass.
     * It is to get it to compile!
     */
    std::unordered_map<std::string, int> answer;
    HashMap<std::string, int> map1;
    HashMap<std::string, int> map2;
    CHECK_MAP_EQUAL(map1, answer);

    for (const auto& kv_value : vec) {
       answer.insert(kv_value);
       map1.insert(kv_value);
       map2.insert(kv_value);
       CHECK_MAP_EQUAL(map1, answer);
    }

    // create const references to the maps
    // to see if these const references work correctly
    const auto& c_ref_map1 = map1;
    const auto& c_ref_map2 = map2;

    // Check const correct of == and != operator

    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(c_ref_map1 == map2 && map2 == c_ref_map1 && c_ref_map1 == c_ref_map1);
    ASSERT_TRUE(c_ref_map2 == map1 && map1 == c_ref_map2 && c_ref_map2 == c_ref_map2);
    ASSERT_TRUE(c_ref_map2 == c_ref_map1 && c_ref_map1 == c_ref_map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));
    ASSERT_TRUE(!(c_ref_map1 != map2) && !(map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map1));
    ASSERT_TRUE(!(c_ref_map2 != map1) && !(map1 != c_ref_map2) && !(c_ref_map2 != c_ref_map2));
    ASSERT_TRUE(!(c_ref_map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map2));

    map1.erase("A");
    ASSERT_TRUE(map1 != map2 && map2 != map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(c_ref_map1 != map2 && map2 != c_ref_map1 && c_ref_map1 == c_ref_map1);
    ASSERT_TRUE(c_ref_map2 != map1 && map1 != c_ref_map2 && c_ref_map2 == c_ref_map2);
    ASSERT_TRUE(c_ref_map2 != c_ref_map1 && c_ref_map1 != c_ref_map2);
    ASSERT_TRUE(!(map1 == map2) && !(map2 == map1) && !(map1 != map1) && !(map2 != map2));
    ASSERT_TRUE(!(c_ref_map1 == map2) && !(map2 == c_ref_map1) && !(c_ref_map1 != c_ref_map1));
    ASSERT_TRUE(!(c_ref_map2 == map1) && !(map1 == c_ref_map2) && !(c_ref_map2 != c_ref_map2));
    ASSERT_TRUE(!(c_ref_map2 == c_ref_map1) && !(c_ref_map1 == c_ref_map2));

    map2.erase("A");
    ASSERT_TRUE(map1 == map2 && map2 == map1 && map1 == map1 && map2 == map2);
    ASSERT_TRUE(c_ref_map1 == map2 && map2 == c_ref_map1 && c_ref_map1 == c_ref_map1);
    ASSERT_TRUE(c_ref_map2 == map1 && map1 == c_ref_map2 && c_ref_map2 == c_ref_map2);
    ASSERT_TRUE(c_ref_map2 == c_ref_map1 && c_ref_map1 == c_ref_map2);
    ASSERT_TRUE(!(map1 != map2) && !(map2 != map1) && !(map1 != map1) && !(map2 != map2));
    ASSERT_TRUE(!(c_ref_map1 != map2) && !(map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map1));
    ASSERT_TRUE(!(c_ref_map2 != map1) && !(map1 != c_ref_map2) && !(c_ref_map2 != c_ref_map2));
    ASSERT_TRUE(!(c_ref_map2 != c_ref_map1) && !(c_ref_map1 != c_ref_map2));
}
#endif

#if RUN_TEST_4A
TEST(HashMapTest, TEST4A_COPY_CTOR_BASIC) {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;

    // populate our initial map
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }
    // create copies via copy constructor
    HashMap<std::string, int> copy_constructed{map};
    std::unordered_map<std::string, int> copy_constructed_answer{answer};
    CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
    CHECK_MAP_EQUAL(map, answer);


    for (const auto& [key, mapped] : vec) {
        copy_constructed.at(key)++;
        copy_constructed_answer.at(key)++;
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        copy_constructed.erase(key);
        copy_constructed_answer.erase(key);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    copy_constructed.clear();
    copy_constructed_answer.clear();
    CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
    CHECK_MAP_EQUAL(map, answer);

    // check integrity of original map and answer
    for (const auto& [key, mapped] : vec) {
        map.at(key)++;
        answer.at(key)++;
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        map.erase(key);
        answer.erase(key);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
        CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

#if RUN_TEST_4B
TEST(HashMapTest, TEST_4B_COPY_ASSIGN_BASIC) {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }

    HashMap<std::string, int> copy_assign;
    std::unordered_map<std::string, int> copy_assign_answer;
    copy_assign = map;
    copy_assign_answer = answer;
    CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
    CHECK_MAP_EQUAL(map, answer);

    for (const auto& [key, mapped] : vec) {
        copy_assign.at(key)++;
        copy_assign_answer.at(key)++;
        CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        copy_assign.erase(key);
        copy_assign_answer.erase(key);
        CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    copy_assign.clear();
    copy_assign_answer.clear();
    CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
    CHECK_MAP_EQUAL(map, answer);

    // check integrity of original map and answer
    for (const auto& [key, mapped] : vec) {
        map.at(key)++;
        answer.at(key)++;
        CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
        CHECK_MAP_EQUAL(map, answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        map.erase(key);
        answer.erase(key);
        CHECK_MAP_EQUAL(copy_assign, copy_assign_answer);
        CHECK_MAP_EQUAL(map, answer);
    }
}
#endif

#if RUN_TEST_4C
TEST(HashMapTest, TEST_4C_COPY_EDGE) {
    {
        /* 1. When map is empty, copy construct and assign */
        HashMap<std::string, int> map;
        map.rehash(1);
        HashMap<std::string, int> copy_constructed{map};

        std::unordered_map<std::string, int> map_answer;
        map_answer.rehash(1);
        std::unordered_map<std::string, int> copy_constructed_answer{map_answer};

        CHECK_MAP_EQUAL(map, map_answer);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);

        map.insert({"Avery", 3});
        map_answer.insert({"Avery", 3});
        copy_constructed.insert({"Anna", 2});
        copy_constructed_answer.insert({"Anna", 2});

        CHECK_MAP_EQUAL(map, map_answer);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);

        HashMap<std::string, int> copy_assigned;
        std::unordered_map<std::string, int> copy_assigned_answer;
        copy_assigned = copy_constructed;
        copy_assigned_answer = copy_constructed_answer;
        copy_assigned.insert({"Ali", 1});
        copy_assigned_answer.insert({"Ali", 1});

        CHECK_MAP_EQUAL(map, map_answer);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);
        CHECK_MAP_EQUAL(map, map_answer);
        CHECK_MAP_EQUAL(copy_constructed, copy_constructed_answer);

    }
    {
        /* 2. Basic self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        HashMap<std::string, int> map;
        std::unordered_map<std::string, int> answer;
        map = map;
        answer = answer;

        for (const auto& kv_pair : vec) {
            map.insert(kv_pair);
            answer.insert(kv_pair);
        }
        map = map;
        answer = answer;
        CHECK_MAP_EQUAL(map, answer);

        for (const auto& [key, mapped] : vec) {
            map.at(key)++;
            answer.at(key)++;
            map = map;
            answer = answer;
            CHECK_MAP_EQUAL(map, answer);
        }

        for (const auto& key : {"A", "B", "Not found again!"}) {
            map.erase(key);
            answer.erase(key);
            map = map;
            answer = answer;
            CHECK_MAP_EQUAL(map, answer);
        }

        map.clear();
        answer.clear();
        map = map;
        answer = answer;
        CHECK_MAP_EQUAL(map, answer);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }

    {
        /* 3. Expanded self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wself-assign-overloaded"
        HashMap<std::string, int> map1;
        HashMap<std::string, int> map2;
        map1.insert({"A", 1});
        map1.insert({"B", 2});
        map2.insert({"C", 3});
        std::unordered_map<std::string, int> answer1 {{"A", 1}, {"B", 2}};
        std::unordered_map<std::string, int> answer2 {{"C", 3}};

        map1 = map2 = map2 = map1;
        answer1 = answer2 = answer2 = answer1;
        CHECK_MAP_EQUAL(map1, answer1);
        CHECK_MAP_EQUAL(map2, answer2);

        (map1 = map1) = map1 = (map1 = map1 = map1);
        (answer1 = answer1) = answer1 = (answer1 = answer1 = answer1);
        CHECK_MAP_EQUAL(map1, answer1);
        CHECK_MAP_EQUAL(map2, answer2);

        map1 = map2 = map1 = (map1 = map1 = map2) = (map2 = map1) = map1;
        answer1 = answer2 = answer1 = (answer1 = answer1 = answer2) = (answer2 = answer1) = answer1;
        CHECK_MAP_EQUAL(map1, answer1);
        CHECK_MAP_EQUAL(map2, answer2);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }
}
#endif

#if RUN_TEST_4D
TEST(HashMapTest, TEST_4D_MOVE_CTOR_BASIC) {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;

    // populate our initial map
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }
    // create copies via move constructor
    HashMap<std::string, int> move_constructed{std::move(map)};
    std::unordered_map<std::string, int> move_constructed_answer{std::move(answer)};
    CHECK_MAP_EQUAL(move_constructed, move_constructed_answer);

    for (const auto& [key, mapped] : vec) {
        move_constructed.at(key)++;
        move_constructed_answer.at(key)++;
        CHECK_MAP_EQUAL(move_constructed, move_constructed_answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        move_constructed.erase(key);
        move_constructed_answer.erase(key);
        CHECK_MAP_EQUAL(move_constructed, move_constructed_answer);
    }

    move_constructed.clear();
    move_constructed_answer.clear();
    CHECK_MAP_EQUAL(move_constructed, move_constructed_answer);
}
#endif

#if RUN_TEST_4E

TEST(HashMapTest, TEST_4E_MOVE_ASSIGN_BASIC) {
    HashMap<std::string, int> map;
    std::unordered_map<std::string, int> answer;
    for (const auto& kv_pair : vec) {
        map.insert(kv_pair);
        answer.insert(kv_pair);
    }

    HashMap<std::string, int> move_assign;
    std::unordered_map<std::string, int> move_assign_answer;
    move_assign = std::move(map);
    move_assign_answer = std::move(answer);
    CHECK_MAP_EQUAL(move_assign, move_assign_answer);

    for (const auto& [key, mapped] : vec) {
        move_assign.at(key)++;
        move_assign_answer.at(key)++;
        CHECK_MAP_EQUAL(move_assign, move_assign_answer);
    }

    for (const auto& key : {"A", "B", "Not found again!"}) {
        move_assign.erase(key);
        move_assign_answer.erase(key);
        CHECK_MAP_EQUAL(move_assign, move_assign_answer);
    }

    move_assign.clear();
    move_assign_answer.clear();
    CHECK_MAP_EQUAL(move_assign, move_assign_answer);
}
#endif

#if RUN_TEST_4F
TEST(HashMapTest, TEST_4F_MOVE_EDGE) {
    {
        /* 1. When map is empty, copy construct and assign */
        HashMap<std::string, int> map;
        HashMap<std::string, int> move_constructed{std::move(map)};
        HashMap<std::string, int> move_assign;
        move_assign = std::move(map);

        std::unordered_map<std::string, int> map_answer;
        std::unordered_map<std::string, int> move_constructed_answer{map_answer};
        std::unordered_map<std::string, int> move_assign_answer;
        move_assign_answer = std::move(map_answer);
        CHECK_MAP_EQUAL(move_constructed, move_constructed_answer);
        CHECK_MAP_EQUAL(move_assign, move_assign_answer);
    }
    {
        /* 2. Basic self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wself-move"
        HashMap<std::string, int> map;
        std::unordered_map<std::string, int> answer;

        map = std::move(map);
        answer = std::move(answer);
        CHECK_MAP_EQUAL(map, answer);
        for (const auto& kv_pair : vec) {
            map.insert(kv_pair);
            answer.insert(kv_pair);
        }
        CHECK_MAP_EQUAL(map, answer);
        map = std::move(map);
        CHECK_MAP_EQUAL(map, answer);

        for (const auto& [key, mapped] : vec) {
            map.at(key)++;
            answer.at(key)++;
            map = std::move(map);
            CHECK_MAP_EQUAL(map, answer);
        }

        for (const auto& key : {"A", "B", "Not found again!"}) {
            map.erase(key);
            answer.erase(key);
            map = std::move(map);
            CHECK_MAP_EQUAL(map, answer);
        }

        map.clear();
        answer.clear();
        map = std::move(map);
        answer = std::move(answer);
        CHECK_MAP_EQUAL(map, answer);

        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!
    }

    {
        /* 3. Expanded self-assignment */
        // suppress the really annoying warnings
        #pragma GCC diagnostic push
        // #pragma GCC diagnostic ignored "-Wself-move"
        HashMap<std::string, int> map1;
        HashMap<std::string, int> map2;
        map1.insert({"A", 1});
        map1.insert({"B", 2});
        map2.insert({"C", 3});
        std::unordered_map<std::string, int> answer1 {{"A", 1}, {"B", 2}};
        std::unordered_map<std::string, int> answer2 {{"C", 3}};
        
        map1 = std::move(map2) = std::move(map2) = std::move(map1);
        answer1 = std::move(answer2) = std::move(answer2) = std::move(answer1);
        
        CHECK_MAP_EQUAL(map1, answer1);
        CHECK_MAP_EQUAL(map2, answer2);

        (map1 = std::move(map1)) = std::move(std::move(map1) = (std::move(map1) = map1 = std::move(map1)));
        CHECK_MAP_EQUAL(map1, answer1);
        CHECK_MAP_EQUAL(map2, answer2);
        #pragma GCC diagnostic pop
        // your code could crash at this line due to memory problems!

        // Check that the map is still assignable from.
        HashMap<std::string, int> copy = map1;
        HashMap<std::string, int> temp, new_assign;
        temp = std::move(map1);
        new_assign = std::move(temp);
        CHECK_MAP_EQUAL(new_assign, copy);
    }
}
#endif

#if RUN_TEST_4G
TEST(HashMapTest, TEST_4G_MOVE_CTOR_TIME) {
    HashMap<int, int> empty_map(1);
    HashMap<int, int> tiny_map(1);
    HashMap<int, int> small_map(1);
    HashMap<int, int> big_map(1);
    HashMap<int, int> huge_map(1);

    for (size_t i = 0; i < 10; ++i) {
        tiny_map.insert({i, i});
    }

    for (size_t i = 0; i < 100; ++i) {
        small_map.insert({i, i});
    }

    for (size_t i = 0; i < 1000; ++i) {
        big_map.insert({i, i});
    }

    for (size_t i = 0; i < 10000; ++i) {
        huge_map.insert({i, i});
    }

    // hey compiler, stop optimizing this out. you made me do this
    __attribute__((unused)) auto start_empty = clock_type::now();
    __attribute__((unused)) HashMap<int, int> move_empty = std::move(empty_map);
    __attribute__((unused)) auto end_empty = clock_type::now();

    auto start_tiny = clock_type::now();
    HashMap<int, int> move_assigned_tiny = std::move(tiny_map);
    auto end_tiny = clock_type::now();
    ns tiny_time = std::chrono::duration_cast<ns>(end_tiny - start_tiny);

    auto start_small = clock_type::now();
    HashMap<int, int> move_assigned_small = std::move(small_map);
    auto end_small = clock_type::now();
    ns small_time = std::chrono::duration_cast<ns>(end_small - start_small);

    auto start_big = clock_type::now();
    HashMap<int, int> move_assigned_big = std::move(big_map);
    auto end_big = clock_type::now();
    ns big_time = std::chrono::duration_cast<ns>(end_big - start_big);

    auto start_huge = clock_type::now();
    HashMap<int, int> move_assigned_huge = std::move(huge_map);
    auto end_huge = clock_type::now();
    ns huge_time = std::chrono::duration_cast<ns>(end_huge - start_huge);

    std::cout << "Move    10 elements: " << tiny_time.count() << " ns" << '\n';
    std::cout << "Move   100 elements: " << small_time.count() << " ns" << '\n';
    std::cout << "Move  1000 elements: " << big_time.count() << " ns" << '\n';
    std::cout << "Move 10000 elements: " << huge_time.count() << " ns" << '\n';

    // verify that move assigning N elements takes O(1) time
    // you should be able to easily beat this benchmark if you actually implemented move
    ASSERT_TRUE(3*tiny_time.count() > small_time.count());
    ASSERT_TRUE(3*small_time.count() > big_time.count());
    ASSERT_TRUE(3*big_time.count() > huge_time.count());
}
#endif

#if RUN_TEST_4H
TEST(HashMapTest, TEST_4H_MOVE_ASSIGN_TIME) {
    HashMap<int, int> empty_map(1);
    HashMap<int, int> tiny_map(1);
    HashMap<int, int> small_map(1);
    HashMap<int, int> big_map(1);
    HashMap<int, int> huge_map(1);

    for (size_t i = 0; i < 10; ++i) {
        tiny_map.insert({i, i});
    }

    for (size_t i = 0; i < 100; ++i) {
        small_map.insert({i, i});
    }

    for (size_t i = 0; i < 1000; ++i) {
        big_map.insert({i, i});
    }

    for (size_t i = 0; i < 10000; ++i) {
        huge_map.insert({i, i});
    }

    // hey compiler, stop optimizing this out. you made me do this
    __attribute__((unused)) auto start_empty = clock_type::now();
    HashMap<int, int> move_assigned_empty(1);
    move_assigned_empty = std::move(empty_map);
    __attribute__((unused)) auto end_empty = clock_type::now();

    auto start_tiny = clock_type::now();
    HashMap<int, int> move_assigned_tiny(1);
    move_assigned_tiny = std::move(tiny_map);
    auto end_tiny = clock_type::now();
    ns tiny_time = std::chrono::duration_cast<ns>(end_tiny - start_tiny);

    auto start_small = clock_type::now();
    HashMap<int, int> move_assigned_small(1);
    move_assigned_small = std::move(small_map);
    auto end_small = clock_type::now();
    ns small_time = std::chrono::duration_cast<ns>(end_small - start_small);

    auto start_big = clock_type::now();
    HashMap<int, int> move_assigned_big(1);
    move_assigned_big = std::move(big_map);
    auto end_big = clock_type::now();
    ns big_time = std::chrono::duration_cast<ns>(end_big - start_big);

    auto start_huge = clock_type::now();
    HashMap<int, int> move_assigned_huge(1);
    move_assigned_huge = std::move(huge_map);
    auto end_huge = clock_type::now();
    ns huge_time = std::chrono::duration_cast<ns>(end_huge - start_huge);

    std::cout << "Move    10 elements: " << tiny_time.count() << " ns" << '\n';
    std::cout << "Move   100 elements: " << small_time.count() << " ns" << '\n';
    std::cout << "Move  1000 elements: " << big_time.count() << " ns" << '\n';
    std::cout << "Move 10000 elements: " << huge_time.count() << " ns" << '\n';

    // verify that move assigning N elements takes O(1) time
    // you should be able to easily beat this benchmark if you actually implemented move
    ASSERT_TRUE(3*tiny_time.count() > small_time.count());
    ASSERT_TRUE(3*small_time.count() > big_time.count());
    ASSERT_TRUE(3*big_time.count() > huge_time.count());
}
#endif
