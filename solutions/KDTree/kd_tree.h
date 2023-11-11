/**
 * File: kd_tree.h
 * Author: (your name here)
 * ------------------------
 * An interface representing a kd-tree in some number of dimensions. The tree
 * can be constructed from a set of data and then queried for membership and
 * nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include <map>
#include "point.h"
#include "math.h"
#include "bounded_priority_queue.h"

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.

template <size_t N, typename ElemType>
class KDTree {
public:
    struct Node {
        Point<N> point;
        ElemType element;
        Node* left_node;
        Node* right_node;
    };
    
    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree();
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree();
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const;
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const;
    bool empty() const;
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const;
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value);
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt);
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
    // TODO: Add implementation details here.
    size_t tree_size;
    Node* root_node;
    
    Node* copy_tree(Node* source);
    void delete_tree(Node* root);
    Node* insert_node(Node*& root, const Point<N>& point, const ElemType& elem, int idx);
    Node* find_node(Node* root, const Point<N>& point, int idx) const;
    void find_knn(BoundedPriorityQueue<ElemType>* bpq, Node* root, const Point<N>& pt, int idx) const;

};

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::delete_tree(Node* root) {
    if (root == NULL) return;
    delete_tree(root->left_node);
    delete_tree(root->right_node);
    delete root;
    root = NULL;
}

template <size_t N, typename ElemType>
typename KDTree<N, ElemType>::Node* KDTree<N, ElemType>::insert_node(Node*& root, const Point<N>& point, const ElemType& elem, int idx) {
    if (root == NULL) {
        root = new Node;
        *root = {point, elem, NULL, NULL};
        tree_size++;
        return root;
    }
    else {
        if (point == root->point) {
            root->element = elem;
            return root;
        } else if (point[idx] < root->point[idx]) {
            return insert_node(root->left_node, point, elem, (idx+1)%N);
        } else {
            return insert_node(root->right_node, point, elem, (idx+1)%N);
        }
    }
}


template <size_t N, typename ElemType>
typename KDTree<N, ElemType>::Node* KDTree<N, ElemType>::find_node(Node* root, const Point<N>& point, int idx) const{
    if (root == NULL) return root;
    if (root->point == point) return root;
    else {
        int nxt_idx = (idx+1)%N;
        if(point[idx] < root->point[idx]) {
            return find_node(root->left_node, point, nxt_idx);
        }
        else {
            return find_node(root->right_node, point, nxt_idx);
        }
    }
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::find_knn(BoundedPriorityQueue<ElemType>* bpq, Node* root, const Point<N>& pt, int idx) const {
    if (root == NULL) return;
    bpq->enqueue(root->element, Distance(pt, root->point));
    int nxt_idx = (idx + 1) % N;
    if (pt[idx] < root->point[idx]) {
        find_knn(bpq, root->left_node, pt, nxt_idx);
    } else {
        find_knn(bpq, root->right_node, pt, nxt_idx);
    }
    
    double distance = fabs(root->point[idx] - pt[idx]);
    if (bpq->worst() > distance || bpq->size() < N) {
        if (pt[idx] < root->point[idx]) {
            find_knn(bpq, root->right_node, pt, nxt_idx);
        } else {
            find_knn(bpq, root->left_node, pt, nxt_idx);
        }
    }
}

template<size_t N, typename ElemType>
typename KDTree<N,ElemType>::Node* KDTree<N, ElemType>::copy_tree(Node* src) {
    if (src == NULL) return NULL;
    Node* copy_node = new Node;
    copy_node->element = src->element;
    copy_node->point = src->point;
    copy_node->left_node = copy_tree(src->left_node);
    copy_node->right_node = copy_tree(src->right_node);
    return copy_node;
}

/** KDTree class implementation details */
template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
    tree_size = 0;
    root_node = NULL;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    // TODO: Fill this in.
    delete_tree(root_node);
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    return N;
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {
    // TODO: Fill this in
    return tree_size;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const {
    //TODO: Fill in this
    return tree_size == 0;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const {
    //TODO: Fill in this
    Node* node = find_node(root_node, pt, 0);
    return node != NULL;
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value) {
    //TODO: Fill in this
    insert_node(root_node, pt, value, 0);
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& point) {
    //TODO: fill in this
    ElemType defaultElem {};
    Node* node = find_node(root_node, point, 0);
    if (node == NULL) {
        node = insert_node(root_node, point, defaultElem, 0);
    }
    return node->element;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::at(const Point<N>& point) {
    Node* node = find_node(root_node, point, 0);
    if (node == NULL) {
        throw std::out_of_range("Out of range");
    } else {
        return node->element;
    }
}

template <size_t N, typename ElemType>
const ElemType& KDTree<N, ElemType>::at(const Point<N>& point) const {
    Node* node = find_node(root_node, point, 0);
    if (node == NULL) {
        throw std::out_of_range("Out of range");
    } else {
        return node->element;
    }
}

template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const {
    BoundedPriorityQueue<ElemType>* bpq = new BoundedPriorityQueue<ElemType>(k);
    find_knn(bpq, root_node, key, 0);
    std::map<ElemType, size_t> freq_map;
    while (!bpq->empty()) {
        auto elem = bpq->dequeueMin();
        if (freq_map.count(elem)) {
            freq_map[elem] = freq_map[elem] + 1;
        } else {
            freq_map[elem] = 1;
        }
    }

    ElemType max_freq_elem;
    size_t max_freq = 0;
    for (const auto& [elem, freq]:freq_map) {
        if (freq > max_freq) {
            max_freq = freq;
            max_freq_elem = elem;
        }
    }
    delete bpq;
    return max_freq_elem;
}

template<size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(const KDTree& rhs):tree_size(rhs.tree_size) {
    root_node = copy_tree(rhs.root_node);
}

template<size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree<N, ElemType>& rhs) {
    if (&rhs == this) {
        return *this;
    }
    tree_size = rhs.size();
    delete_tree(root_node);
    root_node = copy_tree(rhs.root_node);
    return *this;
}

#endif // KDTREE_INCLUDED
