/*
* Assignment 2: HashMap template implementation (STARTER CODE)
*      TODO: write a comment here.
*/

#include "hashmap.h"

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : 
    HashMap{kDefaultBuckets} { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
    _size{0},
    _hash_function{hash},
    _buckets_array(bucket_count, nullptr) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
    clear();
}

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() {
    return _size;
}

template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() {
    return size() == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() {
    return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const{
    return _buckets_array.size();
};

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    auto [prev, node_found] = find_node(key);
            if (node_found == nullptr) {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) {
    return find_node(key).second != nullptr;
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() {
    for (auto& curr : _buckets_array) {
        while (curr != nullptr) {
            curr = curr->next;
        }
    }
    _size = 0;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::find(const K& key) {
    return make_iterator(find_node(key).second);
}

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::iterator, bool> HashMap<K, M, H>::insert(const value_type& value) {
    const auto& [key, mapped] = value;
    auto [prev, node_to_edit] = find_node(key);
    size_t index = _hash_function(key) % bucket_count();

    if (node_to_edit != nullptr) {
        return {make_iterator(node_to_edit), false};
    }

    auto temp = new node(value, _buckets_array[index]);
    _buckets_array[index] = temp;

    ++_size;
    return {make_iterator(temp), true};
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
    size_t index = _hash_function(key) % bucket_count();
    node* curr = _buckets_array[index];
    node* prev = nullptr; // if first node is the key, return {nullptr, front}
    while (curr != nullptr) {
        const auto& [found_key, found_mapped] = curr->value;
        if (found_key == key) {
            return {prev, curr};
        }
        prev = curr;
        curr = curr->next;
    }
    return {nullptr, nullptr}; // key not found at all.
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::begin() {
    size_t index = first_not_empty_bucket();
    if (index == bucket_count()) {
        return end();
    }
    return make_iterator(_buckets_array[index]);
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::begin() const {
    // This is called the static_cast/const_cast trick, which allows us to reuse
    // the non-const version of find to implement the const version.
    // The idea is to cast this so it's pointing to a non-const HashMap, which
    // calls the overload above (and prevent infinite recursion).
    // Also note that we are calling the conversion operator in the iterator class!
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H>*>(this)->begin());
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::end() {
    return make_iterator(nullptr);
}

template <typename K, typename M, typename H>
size_t HashMap<K, M, H>::first_not_empty_bucket() const {
    auto isNotNullptr = [ ](const auto& v){
        return v != nullptr;
    };

    auto found = std::find_if(_buckets_array.begin(), _buckets_array.end(), isNotNullptr);
    return found - _buckets_array.begin();
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::make_iterator(node* curr) {
    if (curr == nullptr) {
        return {&_buckets_array, curr, bucket_count()};
    }
    size_t index = _hash_function(curr->value.first) % bucket_count();
    return {&_buckets_array, curr, index};
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr) {
        return false;
    }
    size_t index = _hash_function(key) % bucket_count();
    (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
    --_size;
    return true;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::erase(typename HashMap<K, M, H>::const_iterator pos) {
    erase(pos++->first);
    return make_iterator(pos._node); // unfortunately we need a regular iterator, not a const_iterator
}

template <typename K, typename M, typename H>
    void HashMap<K, M, H>::debug() {
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
          << "Printing debug information for your HashMap implementation\n"
          << "Size: " << size() << std::setw(15) << std::right
          << "Buckets: " << bucket_count() << std::setw(20) << std::right
          << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

    for (size_t i = 0; i < bucket_count(); ++i) {
        std::cout << "[" << std::setw(3) << i << "]:";
        node* curr = _buckets_array[i];
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            // next line will not compile if << not supported for K or M
            std::cout <<  " -> " << key << ":" << mapped;
            curr = curr->next;
        }
        std::cout <<  " /" <<  '\n';
    }
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ');
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
if (new_bucket_count == 0) {
    throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
}

std::vector<node*> new_buckets_array(new_bucket_count, nullptr);
    for (auto& curr : _buckets_array) { // short answer question is asking about this 'curr'
        while (curr != nullptr) {
            const auto& [key, mapped] = curr->value;
            size_t index = _hash_function(key) % new_bucket_count;

            auto temp = curr;
            curr = temp->next;
            temp->next = new_buckets_array[index];
            new_buckets_array[index] = temp;
        }
    }
    _buckets_array = std::move(new_buckets_array);
}

template <typename K, typename M, typename H>
template <typename InputIt>
HashMap<K, M, H>::HashMap(InputIt first, InputIt last, size_t bucket_count, const H& hash) :
   HashMap(bucket_count, hash) {
   for (auto iter = first; iter != last; ++iter) {
      insert(*iter);
   }
}

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H& hash) :
   HashMap{init.begin(), init.end(), bucket_count, hash} {
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
   return insert({key, {}}).first->second;
}

template <typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
   return lhs.size() == rhs.size() && std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
   return !(lhs == rhs);
}

template <typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& os, const HashMap<K, M, H>& rhs) {
   std::ostringstream oss("{", std::ostringstream::ate);
   for (const auto& [key, value] : rhs) {
       oss << key << ":" << value << ", ";
   }
   std::string s = oss.str();
   os << s.substr(0, s.length()-2) << "}";
   return os;
}

/* Begin Milestone 2: Special Member Functions */

/* end student code */
