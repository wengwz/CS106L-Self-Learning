
#ifndef HASHMAP_ITERATOR_H
#define HASHMAP_ITERATOR_H

#include <iterator>     // for std::forward_iterator_tag
#include <functional>   // for std::conditional_t

// forward declaration for the HashMap class
template <typename K, typename M, typename H> class HashMap;

/*
* Template class for a HashMapIterator
*
* Map = the type of HashMap this class is an iterator for.
* IsConst = whether this is a const_iterator class.
*
* Concept requirements:
* - Map must be a valid class HashMap<K, M, H>
*/
template <typename Map, bool IsConst = true>
class HashMapIterator {
public:

    /*
    * This alias is very important. When dealing with const_iterator, the value_type is always const, and
    * that prevents the client from modifying the elements via a const_iterator. The meta-function
    * std::conditional_t changes the value_type (at compile-time) to a const one if IsConst is true.
    */
    using value_type = std::conditional_t<IsConst, typename Map::value_type, typename Map::value_type>;

    /*
    * Public aliases for this iterator class. Important so STL functions like std::iterator_traits
    * can parse this class for important information, like its iterator category.
    */
    using iterator_category =   std::forward_iterator_tag;
    using difference_type   =   std::ptrdiff_t;
    using pointer           =   value_type*;
    using reference         =   value_type&;

    /*
    * Friend declarations so the HashMap class this iterator is for can access the attributes of its iterators.
    * Also, to make conversions between iterator and const_iterator easy, we declare the corresponding
    * iterator and const_iterators as friends.
    */
    friend Map;
    friend HashMapIterator<Map, true>;
    friend HashMapIterator<Map, false>;

    /*
    * Conversion operator: converts any iterator (iterator or const_iterator) to a const_iterator.
    *
    * Usage:
    *      iterator iter = map.begin();
    *      const_iterator c_iter = iter;    // implicit conversion
    *
    * Implicit conversion operators are usually frowned upon, because they can cause
    * some unexpected behavior. This is a rare case where a conversion operator is
    * very convenient. Many of the iterator functions in the HashMap class are
    * secretly using this conversion.
    *
    * Note: conversion in the opposite direction (const to non-const) is not safe
    * because that gives the client write access the map itself is const.
    */
    operator HashMapIterator<Map, true>() const {
        return HashMapIterator<Map, true>(_buckets_array, _node, _bucket_idx);
    }

    /*
    * Dereference operators: defines the behavior of dereferencing an iterator.
    *
    * Usage:
    *      auto [key, value] = *iter;
    *      auto value = iter->second;
    *      iter->second = 3; // if iter is a regular iterator (not a const_iterator)
    *
    * Note that dereferencing an invalid or end() iterator is undefined behavior.
    */
    reference operator*();
    pointer operator->();

    /*
    * Increment operators: moves the iterator to point to the next element, or end().
    *
    * Usage:
    *      ++iter;         // prefix
    *      iter++;         // postfix
    *
    * Note: the prefix operator first increments, and the returns a reference to itself (which is incremented).
    * The postfix operator returns a copy of the original iterator, while the iterator itself is incremented.
    *
    * Note that incrementing an invalid or end() iterator is undefined behavior.
    */
    HashMapIterator<Map, IsConst>& operator++();
    HashMapIterator<Map, IsConst> operator++(int);

    /*
    * Equality operator: decides if two iterators are pointing to the same element.
    *
    * Usage:
    *      if (iter == map.end()) {...};
    */
    template <typename Map_, bool IsConst_>
    friend bool operator==(const HashMapIterator<Map_, IsConst_>& lhs, const HashMapIterator<Map_, IsConst_>& rhs);

    /*
     * Inequality operator: decides if two iterators are pointing to different elements.
     *
     * Usage:
     *      if (iter != map.end()) {...};
     */
    template <typename Map_, bool IsConst_>
    friend bool operator!=(const HashMapIterator<Map_, IsConst_>& lhs, const HashMapIterator<Map_, IsConst_>& rhs);

    /*
     * Special member functions: we explicitly state that we want the default compiler-generated functions.
     * Here we are following the rule of zero. You should think about why that is correct.
     */
    HashMapIterator(const HashMapIterator<Map, IsConst>& rhs) = default;
    HashMapIterator<Map, IsConst>& operator=(const HashMapIterator<Map, IsConst>& rhs) = default;

    HashMapIterator(HashMapIterator<Map, IsConst>&& rhs) = default;
    HashMapIterator<Map, IsConst>& operator=(HashMapIterator<Map, IsConst>&& rhs) = default;

private:
    using Node = typename Map::Node;
    using bucket_array_type = typename Map::bucket_array_type;

    /*
    * Instance variable: a pointer to the _buckets_array of the HashMap this iterator is for.
    */
    bucket_array_type* _buckets_array;

    /*
    * Instance variable: pointer to the node that stores the element this iterator is currently pointing to.
    */
    Node* _node;

    /*
    * Instance variable: the index of the bucket that _node is in.
    */
    size_t _bucket_idx;

    /*
    * Private constructor for a HashMapIterator.
    * Friend classes can access the private members of class it is friends with, 
    * so HashMap is able to call HashMapIterator's private constructor 
    * (e.g, in begin()). We want the HashMapIterator constructor to be private 
    * so a client can't randomly construct a HashMapIterator without asking for one 
    * through the HashMap's interface.
    */
    HashMapIterator(bucket_array_type* buckets_array, Node* node, size_t bucket_idx);
};

template<typename Map, bool IsConst>
HashMapIterator<Map, IsConst>::HashMapIterator(bucket_array_type* buckets_array, Node* node, size_t bucket_idx):
    _buckets_array(buckets_array),
    _node(node),
    _bucket_idx(bucket_idx) {};
#endif