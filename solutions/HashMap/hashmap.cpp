
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
    clear();
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const {
    return _size;
}

template<typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const {
    return _size == 0;
}

template<typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const {
    return ((float) _size) / _buckets_array.size();
}

template<typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const {
    return _buckets_array.size();
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const {
    auto [pre_node, cur_node] = find_node(key);
    return cur_node != nullptr;

}

template<typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) {
    auto [pre_node, cur_node] = find_node(key);
    if (cur_node == nullptr) throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    auto& [cur_key, cur_value] = cur_node->value;
    return cur_value;
}

template<typename K, typename M, typename H>
const M& HashMap<K, M, H>::at(const K& key) const {
    return static_cast<const M&>(const_cast<HashMap<K,M,H> *>(this)->at(key));
}

template<typename K, typename M, typename H>
void HashMap<K, M, H>::clear() {
    for (auto& bucket : _buckets_array) {
        while (bucket != nullptr)
        {
            auto temp_bkt = bucket->next;
            delete bucket;
            bucket = temp_bkt;
        } 
    }
    _size = 0;
}

template<typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::iterator, bool> HashMap<K, M, H>::insert(const value_type& kv_pair) {
    auto [pre_node, cur_node] = find_node(kv_pair.first);
    size_t index = _hash_function(kv_pair.first) % _buckets_array.size();
    if (cur_node != nullptr) return {make_iterator(cur_node), false};
    Node* new_node = new Node{kv_pair, nullptr};
    if (pre_node == nullptr) {
        _buckets_array[index] = new_node;
    } else {
        pre_node->next = new_node;
    }
    ++_size;
    return {make_iterator(new_node), true}; 
}

template<typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    size_t index = _hash_function(key) % _buckets_array.size();
    auto [pre_node, cur_node] = find_node(key);
    if (cur_node == nullptr) return false;

    Node * temp = cur_node->next;
    delete cur_node;
    if (pre_node != nullptr) {
        pre_node->next = temp;
    } else {
        _buckets_array[index] = temp;
    }
    
    _size--;
    return true;
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::erase(const_iterator pos) {
    iterator temp = make_iterator(pos._node);
    ++temp;
    if (pos._node != nullptr) {
        erase(pos._node->value.first);
    }
    return temp;
}

template<typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_buckets) {
    if (new_buckets == 0) throw std::out_of_range("HashMap<K,M,H>::rehash: Invalid Input Parameters");
    //if (new_buckets == bucket_count()) return;
    bucket_array_type temp_bkt_array = _buckets_array;

    _buckets_array.clear();
    _buckets_array.resize(new_buckets, nullptr);
    
    for (auto& temp_bkt : temp_bkt_array) {
        while (temp_bkt != nullptr)
        {
            Node* temp = temp_bkt;
            temp_bkt = temp_bkt->next;
            const auto& [key, mapped] = temp->value;
            size_t index = _hash_function(key) % new_buckets;

            temp->next = _buckets_array[index];
            _buckets_array[index] = temp;
        }
        
    }
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::begin() {
    size_t index = first_not_empty_bucket();
    return make_iterator(_buckets_array[index]);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::begin() const {
    return const_cast<HashMap<K, M, H> *>(this)->begin();
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::end() {
    return make_iterator(nullptr);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::end() const {
    return const_cast<HashMap<K, M, H> *>(this)->end();
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::find(const K& key) {
    auto [pre_node, cur_node] = find_node(key);
    return make_iterator(cur_node);
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::find(const K& key) const {
    return const_cast<HashMap<K, M, H> *>(this)->find(key);
}

template<typename K, typename M, typename H>
void HashMap<K, M, H>::debug() {
    std::cout << "HashMap Debug Info:" << std::endl;
    std::cout << "Bucket Count=" << bucket_count() <<" Size=" << size() << " Load Factor="<<load_factor()<< std::endl;
    for (size_t i = 0; i < _buckets_array.size(); i++) {
        std::cout << "Bucket-" << i << ": ";
        Node* temp = _buckets_array[i];
        while (temp != nullptr)
        {
            std::cout << temp->value.first << "-" << temp->value.second << " ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }
}

template<typename K, typename M, typename H>
template<typename InputIter>
HashMap<K, M, H>::HashMap(InputIter begin, InputIter end, size_t bucket_count, const H& hash):HashMap(bucket_count, hash){
    for (InputIter iter = begin; iter != end; iter++) {
        insert(*iter);
    }
}

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H& hash):
    HashMap(init.begin(), init.end(), bucket_count, hash){}

template<typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    value_type default_kv{key, {}};
    auto [iter, success] = insert(default_kv);
    return iter->second;
}

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H>& map): 
    _size(0), 
    _hash_function(map._hash_function),
    _buckets_array(map.bucket_count(), nullptr){

    for (const auto& kv_pair : map) {
        insert(kv_pair);
    }
}

template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H>&& map):
    _size(std::move(map._size)),
    _hash_function(std::move(map._hash_function)),
    _buckets_array(std::move(map._buckets_array)) {

    map._buckets_array.resize(_buckets_array.size(), nullptr);
    map._size = 0;
}

template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(const HashMap<K, M, H>& map) {
    if (this == &map) return *this;
    clear();
    _hash_function = map._hash_function;
    for(const auto& kv_pair : map) {
        insert(kv_pair);
    }
    return *this;
}

template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(HashMap<K, M, H>&& map) {
    if (this == &map) return *this;
    clear();
    _size = std::move(map._size);
    _hash_function = map._hash_function;
    _buckets_array = std::move(map._buckets_array);

    map._size = 0;
    map._buckets_array.resize(_buckets_array.size(), nullptr);

    return *this;
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const{
    size_t index = _hash_function(key) % _buckets_array.size();
    Node* pre_node = nullptr;
    Node* cur_node = _buckets_array[index];
    while (cur_node != nullptr)
    {
        const auto& [cur_key, cur_val] = cur_node->value;
        if (cur_key == key) return {pre_node, cur_node};
        pre_node = cur_node;
        cur_node = cur_node->next;
    }
    return {pre_node, cur_node};
}

template<typename K, typename M, typename H>
size_t HashMap<K, M, H>::first_not_empty_bucket() const {
    for (size_t i = 0; i < _buckets_array.size(); i++) {
        if (_buckets_array[i] != nullptr) return i;
    }
    return _buckets_array.size() - 1;
}

template<typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::make_iterator(Node* curr) {
    size_t index = bucket_count();
    if (curr != nullptr) {
        index = _hash_function(curr->value.first) % _buckets_array.size();
    }

    return iterator(&_buckets_array, curr, index);

}

template<typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& stream, const HashMap<K, M, H>& map) {
    std::stringstream str_stream;
    for (const auto& kv_pair : map) {
        str_stream << kv_pair.first << ":" << kv_pair.second << ", ";
    }
    std::string str = str_stream.str();
    if (str.size() != 0) {
        str.erase(str.size() - 2, 2);
    }

    stream << "{" << str << "}";
    return stream;
}

template<typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    for(const auto& kv_pair : lhs) {
        if (!rhs.contains(kv_pair.first)) return false;
        if (rhs.at(kv_pair.first) != kv_pair.second) return false;
    }
    return lhs.size() == rhs.size();
}

template<typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    return !(lhs == rhs);
}
