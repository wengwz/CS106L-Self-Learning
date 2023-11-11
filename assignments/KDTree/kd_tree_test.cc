/*************************************************
 * File: kd_tree_test.cpp 
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * File containing several test cases that can be
 * used to verify the correctness of the KDTree
 * implementation.  While I've tried to exercise
 * as many aspects of the KDTree as possible here,
 * there are almost certainly cases I haven't
 * thought of.  You should make sure to do your own
 * testing in addition to ensuring that the test
 * cases here pass.
 */
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstdarg>
#include <set>
#include "kd_tree.h"
#include "gtest/gtest.h"

/* These flags control which tests will be run.  Initially, only the
 * basic test will be executed.  As you complete more and more parts
 * of the implementation, you will want to turn more and more of these
 * flags on.
 */
#define BasicKDTreeTestEnabled          0 // Step one checks
#define ModerateKDTreeTestEnabled       0
#define HarderKDTreeTestEnabled         0
#define EdgeCaseKDTreeTestEnabled       0
#define MutatingKDTreeTestEnabled       0
#define ThrowingKDTreeTestEnabled       0
#define ConstKDTreeTestEnabled          0

#define NearestNeighborTestEnabled      0 // Step two checks
#define MoreNearestNeighborTestEnabled  0

#define BasicCopyTestEnabled            0 // Step three checks
#define ModerateCopyTestEnabled         0

/* A utility function to construct a Point from a range of iterators. */
template <size_t N, typename IteratorType>
Point<N> PointFromRange(IteratorType begin, IteratorType end) {
  Point<N> result;
  copy(begin, end, result.begin());
  return result;
}

/* Utility functions to create 1-, 2-, 3-, or 4-Points from values. */
Point<1> MakePoint(double x) {
  Point<1> result;
  result[0] = x;
  return result;
}
Point<2> MakePoint(double x, double y) {
  Point<2> result;
  result[0] = x;
  result[1] = y;
  return result;
}
Point<3> MakePoint(double x, double y, double z) {
  Point<3> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  return result;
}
Point<4> MakePoint(double x, double y, double z, double w) {
  Point<4> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  result[3] = w;
  return result;
}


#if BasicKDTreeTestEnabled
TEST(KDTreeTest, BasicKDTreeTest) {
    /* Check basic properties of the KDTree. */
    KDTree<3, size_t> kd;
    EXPECT_EQ(kd.dimension(), 3) << "Expected dimension is 3.";
    EXPECT_EQ(kd.size(), 0) << "New KD tree has no elements.";
    EXPECT_TRUE(kd.empty()) << "New KD tree is empty.";

    /* Add some elements. */
    const double dataPoints[3][3] = { 
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    for (size_t i = 0; i < 3; ++i)
        kd.insert(PointFromRange<3>(dataPoints[i], dataPoints[i] + 3), i);

    /* Check basic properties again. */
    EXPECT_EQ(kd.size(), 3) << "After adding three elements, KDTree has size 3.";
    EXPECT_FALSE(kd.empty()) << "After adding three elements, KDTree is not empty.";

    /* Make sure that the elements we built the tree out of are still there. */
    EXPECT_TRUE(kd.contains(PointFromRange<3>(dataPoints[0], dataPoints[0] + 3))) << "New KD tree has element zero.";
    EXPECT_TRUE(kd.contains(PointFromRange<3>(dataPoints[1], dataPoints[1] + 3))) << "New KD tree has element one.";
    EXPECT_TRUE(kd.contains(PointFromRange<3>(dataPoints[2], dataPoints[2] + 3))) << "New KD tree has element two.";

    /* Make sure that the values of these points are correct. */
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(kd.at(PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)), i) << "New KD tree has correct values.";
    }

}
#endif


/* A trickier test that involves looking up nonexistent elements and working with a
 * larger data set.
 */
#if ModerateKDTreeTestEnabled
TEST(KDTreeTest, ModerateKDTreeTest) {
    /* Build the data set. */
    const double dataPoints[][4] = { 
        {0, 0, 0, 0},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 1, 0, 0},
        {0, 1, 0, 1},
        {0, 1, 1, 0},
        {0, 1, 1, 1},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
        {1, 0, 1, 0},
        {1, 0, 1, 1},
        {1, 1, 0, 0},
        {1, 1, 0, 1},
        {1, 1, 1, 0},
        {1, 1, 1, 1},
    };

    KDTree<4, size_t> kd;
    for (size_t i = 0; i < 16; ++i)
        kd.insert(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4), i);

    /* Check that basic properties hold. */
    EXPECT_EQ(kd.dimension(), 4) << "Expected dimension is 4";
    EXPECT_EQ(kd.size(), 16) << "Expected tree size is 16";
    EXPECT_FALSE(kd.empty()) << "New KD tree is nonempty";


    /* Make sure that the values of these points are correct. */
    for (size_t i = 0; i < 16; ++i)
        EXPECT_EQ(kd.at(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4)), i);

    /* Try looking up some nonexistent elements and see what happens. */
    EXPECT_FALSE(kd.contains(MakePoint(1.0, 1.0, 1.0, 0.5)));
    EXPECT_FALSE(kd.contains(MakePoint(0.0, 0.0, 0.0, -0.5)));

}
#endif


/* This test still uses just the basic functionality, but it hammers it a bit more
 * by checking for strange edge cases like duplicated elements.
 */
#if HarderKDTreeTestEnabled
TEST(KDTreeTest, HarderKDTreeTest) {
    /* Build the data set. */
    const double dataPoints[][4] = { 
        {0, 0, 0, 0},
        {0, 1, 0, 1},
        {0, 0, 0, 0}, // Duplicate!
        {0, 1, 0, 1}, // Duplicate!
        {0, 1, 1, 0},
        {1, 0, 1, 0},   
    };
    KDTree<4, size_t> kd;
    for (size_t i = 0; i < 6; ++i)
        kd.insert(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4), i);

    /* Check basic properties. */
    EXPECT_EQ(kd.dimension(), 4) << "Expected KDTree dimension is 4";
    EXPECT_EQ(kd.size(), 4) << "Expected KDTree size is 4";
    EXPECT_FALSE(kd.empty());

    /* Make sure that elements are still there, without checking values. */
    for (size_t i = 0; i < 6; ++i)
        EXPECT_TRUE(kd.contains(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4))) << "New KD tree has original elems.";

    /* Check that the elements have the correct keys.  Elements [2, 6) should have the
    * correct keys, but elements 0 and 1 will have the keys of elements 2 and 3 because
    * they were overwritten.
    */
    for (size_t i = 2; i < 6; ++i)
        EXPECT_EQ(kd.at(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4)), i);
    for (size_t i = 0; i < 2; ++i)
        EXPECT_EQ(kd.at(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4)), i + 2);

}
#endif

/* This test builds a KDTree where the data has the same values everywhere except 
 * along one coordinate axis.  If you are failing this test case, make sure that
 * your implementation of find() descends into the left subtree only if the current
 * coordinate is _strictly less_ than the partition point's coordinate.
 */
#if EdgeCaseKDTreeTestEnabled

TEST(KDTreeTest, EdgeCaseKDTreeTest) {
    /* Build the data set. */
    const double dataPoints[8][3] = { 
        {0, 0, 0},
        {0, 1, 0},
        {0, 2, 0},
        {0, 3, 0},
        {0, 4, 0},
        {0, 5, 0},
        {0, 6, 0},
        {0, 7, 0},
    };

    /* Add points. */
    KDTree<3, size_t> kd;
    for (size_t i = 0; i < 8; ++i)
        kd.insert(PointFromRange<3>(dataPoints[i], dataPoints[i] + 3), i);

    /* Basic checks. */
    EXPECT_EQ(kd.dimension(), 3) << "Expected dimension of KDTree is 3";
    EXPECT_EQ(kd.size(), 8) << "Expected number of elements in KDTree is 8";
    EXPECT_FALSE(kd.empty());

    /* Make sure everything can be found. */
    for (size_t i = 0; i < kd.size(); ++i) 
        EXPECT_TRUE(kd.contains(PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)));
}

#endif

/* This test actively mutates the elements of the KDTree using
 * operator[].  If you are failing this test, check to make sure
 * that your implementation of operator[] correctly allows for
 * mutation and that it inserts elements if they don't already
 * exist.
 */
#if MutatingKDTreeTestEnabled
TEST(KDTreeTest, MutatingKDTreeTest) {
    /* Build the data set. */
    const double dataPoints[8][3] = { 
        {0, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
        {1, 1, 1},
    };

    /* Add points using []. */
    KDTree<3, size_t> kd;
    for (size_t i = 0; i < 8; ++i)
        kd[PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)] = i;

    /* Basic checks. */
    EXPECT_EQ(kd.dimension(), 3);
    EXPECT_EQ(kd.size(), 8);
    EXPECT_FALSE(kd.empty());

    /* Make sure everything can be found. */
    for (size_t i = 0; i < kd.size(); ++i)
        EXPECT_TRUE(kd.contains(PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)));

    /* Change every other element to have key 0. */
    for (size_t i = 0; i < 8; i += 2)
        kd[PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)] = 0;

    /* Check that the keys are right. */
    for (size_t i = 1; i < 8; i += 2)
        EXPECT_EQ(kd[PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)], i);

    /* Check that the keys are right. */
    for (size_t i = 0; i < 8; i += 2)
        EXPECT_EQ(kd[PointFromRange<3>(dataPoints[i], dataPoints[i] + 3)], 0);
}
#endif

/* This test checks that the at() operators correctly throw
 * exceptions when elements are not found.  This is not an
 * exhaustive test, and you should be sure to confirm on your
 * own that everything works correctly.
 */
#if ThrowingKDTreeTestEnabled
    /* Create a non-const, empty KDTree and look things up. */
TEST(KDTreeTest, ThrowingKDTreeTest) {
    KDTree<3, size_t> empty;

    bool didThrow = false;
    try {
        empty.at(MakePoint(0, 0, 0));
    } catch (const out_of_range&) {
        didThrow = true;
    }
    EXPECT_TRUE(didThrow) << "KDTree is expected to throw exception";

    const KDTree<3, size_t> const_empty;
    didThrow = false;
    try {
        const_empty.at(MakePoint(0, 0, 0));
    } catch (const out_of_range&) {
        didThrow = true;
    }
    EXPECT_TRUE(didThrow) << "Const KDTree is expected to throw exception";
}
#endif


/* A basic test that creates a const KDTree and a non-const KDTree to ensure
 * the class still compiles properly. It also tests the the const version of
 * at is working correctly on the basic KDTree tests.
 */
#if ConstKDTreeTestEnabled
TEST(KDTreeTest, ConstKDTreeTest) {
    /* Build the data set. */
    const double dataPoints[][4] = { 
        {0, 0, 0, 0},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 0, 1, 1}
    };

    KDTree<4, size_t> kd;
    for (size_t i = 0; i < 4; ++i)
        kd.insert(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4), i);
    
    /* Check that the code compiles for the non-const version. */
    kd.dimension();
    kd.size();
    kd.empty();
    kd.at(PointFromRange<4>(dataPoints[0], dataPoints[0] + 4)) = 100;

    const KDTree<4, size_t>& const_kd = kd;
    
    /* Check that the code compiles for the const version. */
    const_kd.dimension();
    const_kd.size();
    const_kd.empty();
    const_kd.at(PointFromRange<4>(dataPoints[0], dataPoints[0] + 4));
    
    EXPECT_TRUE(true) << "Const code compiles";
    
    /* Run the basic KD Tree tests using a const KD Tree. */
    EXPECT_TRUE(const_kd.contains(PointFromRange<4>(dataPoints[0], dataPoints[0] + 4)));
    EXPECT_TRUE(const_kd.contains(PointFromRange<4>(dataPoints[1], dataPoints[1] + 4)));
    EXPECT_TRUE(const_kd.contains(PointFromRange<4>(dataPoints[2], dataPoints[2] + 4)));
    EXPECT_TRUE(const_kd.contains(PointFromRange<4>(dataPoints[3], dataPoints[3] + 4)));
    
    /* Make sure that the values of these points are correct. */
    EXPECT_EQ(const_kd.at(PointFromRange<4>(dataPoints[0], dataPoints[0] + 4)), 100);
    for (size_t i = 1; i < 4; ++i)
        EXPECT_EQ(const_kd.at(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4)), i);
}
#endif

/* This is a simple test case that checks whether 1-NN lookups work
 * correctly.  If you're failing this test case, you probably need
 * to check that your implementation of kNearestNeighborValue works
 * correctly.
 */

#if NearestNeighborTestEnabled
TEST(KDTreeTest, NearestNeighborTest) {
    /* Build two data sets - a set of tree points and a set of test points. */
    const double dataPoints[][4] = { 
        {0, 0, 0, 0}, // 0
        {0, 0, 0, 1}, // 1
        {0, 0, 1, 0}, // 2
        {0, 0, 1, 1}, // 3
        {0, 1, 0, 0}, // 4
        {0, 1, 0, 1}, // 5
        {0, 1, 1, 0}, // 6
        {0, 1, 1, 1}, // 7
        {1, 0, 0, 0}, // 8
        {1, 0, 0, 1}, // 9
        {1, 0, 1, 0}, // 10
        {1, 0, 1, 1}, // 11
        {1, 1, 0, 0}, // 12
        {1, 1, 0, 1}, // 13
        {1, 1, 1, 0}, // 14
        {1, 1, 1, 1}, // 15
    };

    /* Each test point should be right next to the corresponding point in the
    * tree point set.
    */
    const double testPoints[][4] = { 
        {0, 0, 0, 0},          // 0
        {0, 0, 0, 0.7},        // 1
        {0, 0, 0.9, 0},        // 2
        {0, 0, 0.6, 0.6},      // 3
        {0, 0.9, 0, 0},        // 4
        {0, 0.8, 0, 0.7},      // 5
        {0, 0.6, 0.7, -0.1},   // 6
        {-0.4, 0.7, 0.7, 0.7}, // 7
        {1, 0, 0, 0},          // 8
        {1, 0, 0, 1},          // 9
        {1, 0, 1, 0},          // 10
        {1, 0, 1, 1},          // 11
        {1, 1, 0, 0},          // 12
        {1, 1, 0, 1},          // 13
        {1, 1, 1, 0},          // 14
        {1, 1, 1, 1},          // 15
    };

    /* Build up a data set from the first set of points. */
    KDTree<4, size_t> kd;
    for (size_t i = 0; i < 16; ++i)
        kd.insert(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4), i);

    /* Check that calling nearest neighbor on a point in the tree yields that point. */
    for (size_t i = 0; i < 16; ++i)
        EXPECT_EQ(kd.kNNValue(PointFromRange<4>(dataPoints[i], dataPoints[i] + 4), 1), i) << "Nearest neighbor of element is that element.";

    /* Check that calling nearest neighbor on the test points yields the correct tree points. */
    for (size_t i = 0; i < 16; ++i)
        EXPECT_EQ(kd.kNNValue(PointFromRange<4>(testPoints[i], testPoints[i] + 4), 1), i) << "Test point yielded correct nearest neighbor.";
}
#endif

/* This test verifies that k-NN works for k > 1.  If you're failing this test case, then you probably
 * need to check that you're computing the majority element correctly.
 */
#if MoreNearestNeighborTestEnabled
TEST(KDTreeTest, MoreNearestNeighborTest) {
    /* We're going to build a 3x3 lattice that looks like this:
    *
    * a b a
    * b a b
    * b b b
    *
    * This will give us some fun checking whether k-NN works
    * correctly.
    */
    vector< pair<Point<2>, char> > values;
    values.push_back(make_pair(MakePoint(0.0, 0.0), 'a'));
    values.push_back(make_pair(MakePoint(0.0, 0.5), 'b'));
    values.push_back(make_pair(MakePoint(0.0, 1.0), 'a'));
    values.push_back(make_pair(MakePoint(0.5, 0.0), 'b'));
    values.push_back(make_pair(MakePoint(0.5, 0.5), 'a'));
    values.push_back(make_pair(MakePoint(0.5, 1.0), 'b'));
    values.push_back(make_pair(MakePoint(1.0, 0.0), 'b'));
    values.push_back(make_pair(MakePoint(1.0, 0.5), 'b'));
    values.push_back(make_pair(MakePoint(1.0, 1.0), 'b'));

    /* Build the tree by converting the list to a tree. */
    KDTree<2, char> kd;
    for (size_t i = 0; i < values.size(); ++i) 
        kd[values[i].first] = values[i].second;
    
    /* Look for the the four points closes to the top-middle.  This should give us 'a' even though
    * the point itself is 'b'.
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(0.0, 0.5), 4), 'a') << "Nearest neighbors are correct.";

    /* Check for the nine points closest to the center.  This should give us 'b' even though the
    * point itself is 'a'
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(0.0, 0.5), 9), 'b') << "Nearest neighbors are correct.";

    /* Check for the five points closest to the center.  This should give us 'b' even though the
    * point itself is 'a'
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(0.5, 0.5), 5), 'b') << "Nearest neighbors are correct.";

    /* Look in the center of the square in the bottom-right corner.  All the points are 'b' and
    * so we'd better get 'b' back!
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(0.75, 0.75), 4), 'b') << "Nearest neighbors are correct.";

    /* Choose a point way out of the box and get the 9-NN value.  Since there are only 9 data points,
    * this should be equivalent to asking "what's the most common element?"  (The answer is 'b')
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(+10.0, +10.0), 9), 'b') << "Nearest neighbor at distant point is majority element.";

    /* Similar check but for negative numbers. */
    EXPECT_EQ(kd.kNNValue(MakePoint(-10.0, -10.0), 9), 'b') << "No problems with negative values.";

    /* Try getting the 25 nearest neighbors to a point.  There are only nine points, but this should
    * still work correctly.
    */
    EXPECT_EQ(kd.kNNValue(MakePoint(-10.0, -10.0), 25), 'b') << "No problems with looking up more neighbors than elements.";
}
#endif

/* Tests basic behavior of the copy constructor and assignment operator. */
#if BasicCopyTestEnabled
TEST(KDTreeTest, BasicCopyTest) {
    /* For simplicity, we'll use one-dimensional KDTrees in this step. */
    KDTree<1, size_t> one;
    for (size_t i = 0; i < 10; ++i)
        one[MakePoint(2 * i)] = i; // Load with 0, 2, 4, ..., 18
        
    {
        /* Create a clone of one and confirm that everything copied correctly. 
        * This uses the copy constructor.
        */
        KDTree<1, size_t> clone = one;

        /* Basic checks. */
        EXPECT_EQ(one.size(), clone.size()) << "Copy has the same number of elements as the original.";
        EXPECT_EQ(one.empty(), clone.empty()) << "Copy and original agree on emptiness.";
        EXPECT_EQ(one.dimension(), clone.dimension()) << "Copy and original agree on dimension.";

        /* Check that everything in one is there. */
        for (size_t i = 0; i < 10; ++i)
            EXPECT_EQ(clone.at(MakePoint(2 * i)), i) << "Element from original present in copy.";

        /* Check that nothing else is. */
        for (size_t i = 0; i < 10; ++i)
            EXPECT_FALSE(clone.contains(MakePoint(2 * i + 1))) << "Other elements not present in copy.";        
    }
    
    {
        /* Create a clone of one and confirm that everything copied correctly. 
        * This uses the assignment operator.
        */
        KDTree<1, size_t> clone;
        clone = one;

        /* Basic checks. */
        EXPECT_EQ(one.size(), clone.size()) << "Copy has the same number of elements as the original.";
        EXPECT_EQ(one.empty(), clone.empty()) << "Copy and original agree on emptiness.";
        EXPECT_EQ(one.dimension(), clone.dimension()) << "Copy and original agree on dimension.";

        /* Check that everything in one is there. */
        for (size_t i = 0; i < 10; ++i)
            EXPECT_EQ(clone.at(MakePoint(2 * i)), i) << "Element from original present in copy.";

        /* Check that nothing else is. */
        for (size_t i = 0; i < 10; ++i)
            EXPECT_FALSE(clone.contains(MakePoint(2 * i + 1))) << "Other elements not present in copy.";
    }
}

#endif

/* A more merciless test of copy behavior.. */
#if ModerateCopyTestEnabled
TEST(KDTreeTest, ModerateCopyTest) {
    /* For simplicity, we'll use one-dimensional KDTrees in this step. */
    KDTree<1, size_t> one;
    for (size_t i = 0; i < 10; ++i)
        one[MakePoint(2 * i)] = i; // Load with 0, 2, 4, ..., 18

    {
        /* Create a clone of one and confirm that everything copied correctly. 
        * This uses the copy constructor.
        */
        KDTree<1, size_t> clone = one;

        /* Add odd numbers to the clone. */
        for (size_t i = 0; i < 10; ++i)
        clone[MakePoint(2 * i + 1)] = i;

        /* Confirm that they didn't appear in one. */
        EXPECT_EQ(one.size(), 10) << "Adding to clone change original size.";
        for (size_t i = 0; i < 10; ++i)
            EXPECT_FALSE(one.contains(MakePoint(2 * i + 1))) << "Modifying copy doesn't modify original.";
    }

    /* Check the integrity of the original out here as well to see that the dtor didn't hose things. */
    EXPECT_EQ(one.size(), 10) << "After dtor, original is still good.";
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_FALSE(one.contains(MakePoint(2 * i + 1))) << "After dtor, missing elements still missing.";
        EXPECT_EQ(one[MakePoint(2 * i)], i) << "After dtor, original elements are still there.";
    }

    {
        /* Create a clone of one and confirm that everything copied correctly. 
        * This uses the assignment operator.
        */
        KDTree<1, size_t> clone;
        clone = one;

        /* Do awful, awful things to the copy. */
        clone = clone = (clone = clone);
        (clone = one) = clone;
        clone = clone = clone = clone = clone;
    }
}
#endif
