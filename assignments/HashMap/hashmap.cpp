#include "hashmap.h"

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : _size(0), _hash_function(H()), _buckets_array(kDefaultBuckets, nullptr) {};

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash):
    _size(0), 
    _hash_function(hash), 
    _buckets_array(bucket_count, nullptr) {};

template<typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    // TODO: The implementation of contains should be modified
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size(){
    return _size;
}

template<typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty(){
    return _size == 0;
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() {
    return _buckets_array.size();
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) {
    // TODO: The implementation of contains should be modified
    return false;
}

template<typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    // TODO: The implementation of at should be modified
    throw std::runtime_error("at() function not implemented yet");
}