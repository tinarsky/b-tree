#ifndef B_TREE__B_TREE_H_
#define B_TREE__B_TREE_H_

template<std::totally_ordered K, std::copyable V>
class BTree {
  public:
    struct Entry {
        K key;
        V value;

        Entry() = default;

        Entry(K key, V value) : key(key), value(value) {}

        bool operator<(const Entry &other) const {
            if (key < other.key) {
                return true;
            }
            return false;
        }
        bool operator>(const Entry &other) const {
            return other < *this;
        }
        bool operator<=(const Entry &other) const {
            return !(other < *this);
        }
        bool operator>=(const Entry &other) const {
            return !(*this < other);
        }

        bool operator==(const Entry &other) const {
            return key == other.key;
        }
        bool operator!=(const Entry &other) const {
            return !(other == *this);
        }
    };

  private:
    class Node;
    Node *root_;
    const long min_degree_;
    size_t size_;

    class Node {
      private:
        Entry *entries_;
        const long min_degree_;
        Node **children_;
        Node *parent_;
        long number_of_entries_;
        bool is_leaf_;

        Node(long min_degree, Node *parent, bool is_leaf) : min_degree_(
            min_degree),
                                                            parent_(parent),
                                                            is_leaf_(is_leaf),
                                                            number_of_entries_(0) {
            entries_ = new Entry[2 * min_degree_ - 1];
            children_ = new Node *[2 * min_degree_];
        }

        ~Node() {
            if (!is_leaf_) {
                for (long i = 0; i <= number_of_entries_; ++i) {
                    delete (children_[i]);
                }
            }

            delete[] entries_;
            delete[] children_;
        }

        /*
         * the node must be non-full when this function is called
        */
        void insertInNonFull(Entry entry) {
            if (is_leaf_) {
                insertInNonFullLeaf(entry);
                return;
            }

            long ind = number_of_entries_ - 1;

            while (ind >= 0 && entry < entries_[ind]) {
                ind--;
            }

            if (children_[ind + 1]->isNodeFull()) {
                splitChild(ind + 1);

                if (entries_[ind + 1] < entry) {
                    ind++;
                }
            }

            children_[ind + 1]->insertInNonFull(entry);
        }

        void insertInNonFullLeaf(Entry entry) {
            long ind = number_of_entries_ - 1;
            while (ind >= 0 && entry < entries_[ind]) {
                entries_[ind + 1] = entries_[ind];
                ind--;
            }

            entries_[ind + 1] = entry;
            number_of_entries_ = number_of_entries_ + 1;
        }

        [[nodiscard]] bool isNodeFull() const {
            return this->number_of_entries_ == (2 * min_degree_ - 1);
        }

        /*
         * the child must be full when this function is called
        */
        void splitChild(long child_index) {
            Node *new_child = separateNewChild(children_[child_index]);

            children_[child_index]->number_of_entries_ = min_degree_ - 1;

            for (long j = number_of_entries_; j >= (child_index + 1); j--) {
                children_[j + 1] = children_[j];
            }

            children_[child_index + 1] = new_child;

            for (long j = number_of_entries_ - 1; j > 0 && j >= child_index;
                 j--) {
                entries_[j + 1] = entries_[j];
            }

            if (child_index == 0) {
                entries_[1] = entries_[0];
            }

            entries_[child_index] =
                children_[child_index]->entries_[min_degree_ - 1];

            number_of_entries_ = number_of_entries_ + 1;
        }

        Node *separateNewChild(const Node *child) const {
            Node *new_child =
                new Node(child->min_degree_, child->parent_, child->is_leaf_);
            new_child->number_of_entries_ = min_degree_ - 1;

            for (long j = 0; j < min_degree_ - 1; j++) {
                new_child->entries_[j] = child->entries_[j + min_degree_];
            }

            if (!new_child->is_leaf_) {
                for (long j = 0; j < min_degree_; j++) {
                    new_child->children_[j] = child->children_[j + min_degree_];
                    new_child->children_[j]->parent_ = new_child;
                }
            }
            return new_child;
        }

        /*
         * returns the index of the first entry that is greater or equal to entry
        */
        long findUpperBoundEntryIndex(Entry entry) {
            return std::upper_bound(entries_,
                                    entries_ + number_of_entries_,
                                    entry,
                                    [](const Entry &a, const Entry &b) {
                                        return a <= b;
                                    })
                - entries_;
        }

        bool isEntryPresent(Entry entry, long ind) const {
            return ind < number_of_entries_ && entries_[ind] == entry;
        }

        void removeFromLeaf(long ind) {
            for (long i = ind + 1; i < number_of_entries_; ++i) {
                entries_[i - 1] = entries_[i];
            }

            number_of_entries_--;
        }

        void removeFromNonLeaf(long ind) {
            Entry entry = entries_[ind];

            if (children_[ind]->number_of_entries_ >= min_degree_) {
                Entry prev_entry = children_[ind]->getMaxEntryInSubtree();
                entries_[ind] = prev_entry;
                children_[ind]->remove(prev_entry);
                return;
            }

            if (children_[ind + 1]->number_of_entries_ >= min_degree_) {
                Entry next_entry = children_[ind + 1]->getMinEntryInSubtree();
                entries_[ind] = next_entry;
                children_[ind + 1]->remove(next_entry);
                return;
            }

            merge(ind);
            children_[ind]->remove(entry);
        }

        Entry getMaxEntryInSubtree() {
            Node *right_most_leaf = this->getRightMostLeaf();
            return right_most_leaf->entries_[right_most_leaf->number_of_entries_
                - 1];
        }

        Entry getMinEntryInSubtree() {
            return this->getLeftMostLeaf()->entries_[0];
        }

        void fillToMinDegree(long ind) {
            if (ind != 0
                && children_[ind - 1]->number_of_entries_ >= min_degree_) {
                borrowFromPrev(ind);
                return;
            }

            if (ind != number_of_entries_
                && children_[ind + 1]->number_of_entries_ >= min_degree_) {
                borrowFromNext(ind);
                return;
            }

            if (ind != number_of_entries_) {
                merge(ind);
                return;
            }

            merge(ind - 1);
        }

        void borrowFromPrev(long ind) {
            Node *child = children_[ind];
            Node *left_sibling = children_[ind - 1];

            for (long i = child->number_of_entries_ - 1; i >= 0; --i) {
                child->entries_[i + 1] = child->entries_[i];
            }
            child->entries_[0] = entries_[ind - 1];

            if (!child->is_leaf_) {
                for (long i = child->number_of_entries_; i >= 0; --i) {
                    child->children_[i + 1] = child->children_[i];
                }
                child->children_[0] =
                    left_sibling->children_[left_sibling->number_of_entries_];
                child->children_[0]->parent_ = child;
            }

            entries_[ind - 1] =
                left_sibling->entries_[left_sibling->number_of_entries_ - 1];

            child->number_of_entries_++;
            left_sibling->number_of_entries_--;
        }

        void borrowFromNext(long ind) {
            Node *child = children_[ind];
            Node *sibling = children_[ind + 1];

            child->entries_[(child->number_of_entries_)] = entries_[ind];

            if (!child->is_leaf_) {
                child->children_[(child->number_of_entries_) + 1] =
                    sibling->children_[0];
                sibling->children_[0]->parent_ = child;
            }

            entries_[ind] = sibling->entries_[0];

            for (long i = 1; i < sibling->number_of_entries_; ++i) {
                sibling->entries_[i - 1] = sibling->entries_[i];
            }

            if (!sibling->is_leaf_) {
                for (long i = 1; i <= sibling->number_of_entries_; ++i) {
                    sibling->children_[i - 1] = sibling->children_[i];
                }
            }

            child->number_of_entries_++;
            sibling->number_of_entries_--;
        }

        /*
         * A method to merge children_[ind] with children_[ind+1]
         * children_[ind+1] is freed after merging
        */
        void merge(long ind) {
            Node *child = children_[ind];
            Node *sibling = children_[ind + 1];

            child->entries_[min_degree_ - 1] = entries_[ind];

            for (long i = 0; i < sibling->number_of_entries_; ++i) {
                child->entries_[i + min_degree_] = sibling->entries_[i];
            }

            if (!child->is_leaf_) {
                for (long i = 0; i <= sibling->number_of_entries_; ++i) {
                    child->children_[i + min_degree_] = sibling->children_[i];
                    sibling->children_[i] = nullptr;
                    child->children_[i + min_degree_]->parent_ = child;
                }
            }

            for (long i = ind + 1; i < number_of_entries_; ++i) {
                entries_[i - 1] = entries_[i];
            }

            for (long i = ind + 2; i <= number_of_entries_; ++i) {
                children_[i - 1] = children_[i];
            }

            child->number_of_entries_ += (sibling->number_of_entries_ + 1);
            number_of_entries_--;

            delete (sibling);
        }

        Node *copyNode(Node *new_parent) {
            Node *new_node = new Node(min_degree_, new_parent, is_leaf_);
            new_node->number_of_entries_ = number_of_entries_;
            for (long i = 0; i < number_of_entries_; ++i) {
                new_node->entries_[i] = entries_[i];
                if (!is_leaf_) {
                    new_node->children_[i] =
                        children_[i]->copyNode(new_node);
                }
            }
            if (!is_leaf_) {
                new_node->children_[number_of_entries_] =
                    children_[number_of_entries_]->copyNode(new_node);
            }
            return new_node;
        }

      public:

        Node(const Node &node) = delete;

        void traverse(std::ostream &out) const {
            for (long i = 0; i < number_of_entries_; i++) {
                if (!is_leaf_) {
                    children_[i]->traverse(out);
                }
                out << " (" << entries_[i].key << ", "
                    << entries_[i].value << ")";
            }

            // print the subtree rooted with last child
            if (!is_leaf_) {
                children_[number_of_entries_]->traverse(out);
            }
        }

        /*
         * returns nullptr if entry is not present
        */
        Node *search(Entry entry) {
            long ind = findUpperBoundEntryIndex(entry);

            if (isEntryPresent(entry, ind)) {
                return this;
            }

            if (is_leaf_) {
                return nullptr;
            }

            return children_[ind]->search(entry);
        }

        /*
         * returns number of elements removed (0 or 1)
        */
        int remove(Entry entry) {
            long ind = findUpperBoundEntryIndex(entry);

            if (isEntryPresent(entry, ind)) {
                is_leaf_ ? removeFromLeaf(ind) : removeFromNonLeaf(ind);
                return 1;
            }

            if (is_leaf_) {
                return 0;
            }

            if (children_[ind]->number_of_entries_ < min_degree_) {
                fillToMinDegree(ind);
            }

            // this is only true if the last child was merged with the previous child
            if (ind > number_of_entries_) {
                return children_[ind - 1]->remove(entry);
            }

            return children_[ind]->remove(entry);
        }

        // returns -1 if this child is not present
        long getChildIndex(Node *child) {
            long ind = -1;
            for (long i = 0; i < (number_of_entries_ + 1); i++) {
                if (children_[i] == child) {
                    ind = i;
                    break;
                }
            }
            return ind;
        }

        Node *getRightMostLeaf() {
            Node *subtree_root = this;
            while (!subtree_root->is_leaf_) {
                subtree_root =
                    subtree_root->children_[subtree_root->number_of_entries_];
            }

            return subtree_root;
        }

        Node *getLeftMostLeaf() {
            Node *subtree_root = this;
            while (!subtree_root->is_leaf_) {
                subtree_root = subtree_root->children_[0];
            }

            return subtree_root;
        }

        friend class BTree;
    };

    void insertIfRootIsFull(Entry entry) {
        Node *new_root = new Node(min_degree_, nullptr, false);
        new_root->children_[0] = root_;
        root_->parent_ = new_root;
        new_root->splitChild(0);
        root_ = new_root;

        if (entry <= new_root->entries_[0]) {
            new_root->children_[0]->insertInNonFull(entry);
            return;
        }
        new_root->children_[1]->insertInNonFull(entry);
    }

  public:

    // min_degree >= 3
    explicit BTree(long min_degree) : root_(nullptr),
                                      min_degree_(min_degree),
                                      size_(0) {
        if (min_degree < 3) {
            throw std::invalid_argument(
                "min degree must be greater or equal than 3");
        }
    }

    BTree(const BTree<K, V> &other) : root_(other.root_),
                                      min_degree_(other.min_degree_),
                                      size_(other.size_) {
        if (root_ != nullptr) {
            root_ = other.root_->copyNode(nullptr);
        }
    }

    BTree<K, V> &operator=(const BTree<K, V> &other) {
        BTree<K, V> tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(const BTree<K, V> &other) {
        auto tmp_root = root_;
        root_ = other.root_;
        other.root_ = tmp_root;

        auto tmp_size = size_;
        size_ = other.size_;
        other.size_ = tmp_size;

        auto tmp_min_degree = min_degree_;
        min_degree_ = other.min_degree_;
        other.min_degree_ = tmp_min_degree;
    }

    ~BTree() {
        delete root_;
    }

    void traverse(std::ostream &out) const {
        if (root_ != nullptr) { root_->traverse(out); }
    }

    size_t size() {
        return size_;
    }

    void insert(K key, V value) {
        size_++;
        Entry entry(key, value);

        if (root_ == nullptr) {
            root_ = new Node(min_degree_, nullptr, true);
            root_->entries_[0] = entry;
            root_->number_of_entries_ = 1;
            return;
        }

        if (root_->isNodeFull()) {
            insertIfRootIsFull(entry);
            return;
        }

        root_->insertInNonFull(entry);
    }

    /*
     * returns number of elements removed (0 or 1)
    */
    int remove(K key) {
        if (root_ == nullptr) {
            return 0;
        }

        Entry entry;
        entry.key = key;

        int number_of_removed_elems = root_->remove(entry);
        size_ -= number_of_removed_elems;

        if (root_->number_of_entries_ != 0) {
            return number_of_removed_elems;
        }

        Node *old_root = root_;
        root_ = root_->is_leaf_ ? nullptr : root_->children_[0];
        if (root_ != nullptr) {
            root_->parent_ = nullptr;
        }

        for (long i = 0; i <= old_root->number_of_entries_; ++i) {
            old_root->children_[i] = nullptr;
        }
        delete old_root;

        return number_of_removed_elems;
    }

    struct Iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Entry;
        using pointer = Entry *;
        using reference = Entry &;

        void increment() {
            if (ind_ == node_->number_of_entries_) {
                return;
            }

            if (!node_->is_leaf_) {
                node_ = node_->children_[ind_ + 1]->getLeftMostLeaf();
                ind_ = 0;
                return;
            }

            auto leaf = node_;
            ++ind_;
            while (node_->parent_ != nullptr
                && ind_ == node_->number_of_entries_) {
                ind_ = node_->parent_->getChildIndex(node_);
                node_ = node_->parent_;
            }

            if (ind_ == node_->number_of_entries_) {
                ind_ = leaf->number_of_entries_;
                node_ = leaf;
            }
        }

        void decrement() {
            if (!node_->is_leaf_) {
                node_ = node_->children_[ind_]->getRightMostLeaf();
                ind_ = node_->number_of_entries_ - 1;
                return;
            }

            if (ind_ > 0) {
                --ind_;
                return;
            }

            auto leaf = node_;
            while (node_->parent_ != nullptr
                && (ind_ = node_->parent_->getChildIndex(node_)) == 0) {
                node_ = node_->parent_;
            }

            if (ind_ == 0) {
                node_ = leaf;
                return;
            }

            --ind_;
            node_ = node_->parent_;
        }

        Iterator(Node *node, long ind) : node_(node), ind_(ind) {
        }

        reference operator*() const {
            return node_->entries_[ind_];
        }

        pointer operator->() {
            return node_->entries_ + ind_;
        }

        Iterator &operator++() {
            increment();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            increment();
            return temp;
        }

        Iterator &operator--() {
            decrement();
            return *this;
        }

        Iterator operator--(int) {
            auto temp = *this;
            decrement();
            return temp;
        }

        friend bool operator==(const Iterator &first,
                               const Iterator &second) {
            return first.node_ == second.node_ && first.ind_ == second.ind_;
        }

        friend bool operator!=(const Iterator &first,
                               const Iterator &second) {
            return !(first == second);
        }

      private:
        Node *node_;
        long ind_;
    };

    struct ConstIterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Entry;
        using pointer = const Entry *;
        using reference = const Entry &;

        void increment() {
            if (ind_ == node_->number_of_entries_) {
                return;
            }

            if (!node_->is_leaf_) {
                node_ = node_->children_[ind_ + 1]->getLeftMostLeaf();
                ind_ = 0;
                return;
            }

            auto leaf = node_;
            ++ind_;
            while (node_->parent_ != nullptr
                && ind_ == node_->number_of_entries_) {
                ind_ = node_->parent_->getChildIndex(node_);
                node_ = node_->parent_;
            }

            if (ind_ == node_->number_of_entries_) {
                ind_ = leaf->number_of_entries_;
                node_ = leaf;
            }
        }

        void decrement() {
            if (!node_->is_leaf_) {
                node_ = node_->children_[ind_]->getRightMostLeaf();
                ind_ = node_->number_of_entries_ - 1;
                return;
            }

            if (ind_ > 0) {
                --ind_;
                return;
            }

            auto leaf = node_;
            while (node_->parent_ != nullptr
                && (ind_ = node_->parent_->getChildIndex(node_)) == 0) {
                node_ = node_->parent_;
            }

            if (ind_ == 0) {
                node_ = leaf;
                return;
            }

            --ind_;
            node_ = node_->parent_;
        }

        ConstIterator(Node *node, long ind) : node_(node), ind_(ind) {
        }

        reference operator*() const {
            return node_->entries_[ind_];
        }

        pointer operator->() const {
            return node_->entries_ + ind_;
        }

        ConstIterator &operator++() {
            increment();
            return *this;
        }

        ConstIterator operator++(int) {
            auto temp = *this;
            increment();
            return temp;
        }

        ConstIterator &operator--() {
            decrement();
            return *this;
        }

        ConstIterator operator--(int) {
            Iterator temp = *this;
            decrement();
            return temp;
        }

        friend bool operator==(const ConstIterator &first,
                               const ConstIterator &second) {
            return first.node_ == second.node_ && first.ind_ == second.ind_;
        }

        friend bool operator!=(const ConstIterator &first,
                               const ConstIterator &second) {
            return !(first == second);
        }

      private:
        Node *node_;
        long ind_;
    };

    /*
     * returns iterator on this element if present,
     * otherwise returns iterator on end
     */
    Iterator search(K key) {
        if (root_ == nullptr) {
            return end();
        }

        Entry entry;
        entry.key = key;

        Node *node = root_->search(entry);
        if (node == nullptr) {
            return end();
        }

        return Iterator(node, node->findUpperBoundEntryIndex(entry));
    }

    Iterator begin() {
        return Iterator(root_->getLeftMostLeaf(), 0);
    }

    Iterator end() {
        auto right_most_leaf = root_->getRightMostLeaf();
        return Iterator(right_most_leaf, right_most_leaf->number_of_entries_);
    }

    Iterator cbegin() const {
        return ConstIterator(root_->getLeftMostLeaf(), 0);
    }

    Iterator cend() const {
        auto right_most_leaf = root_->getRightMostLeaf();
        return ConstIterator(right_most_leaf,
                             right_most_leaf->number_of_entries_);
    }

    std::reverse_iterator<Iterator> rbegin() {
        return std::reverse_iterator<Iterator>(end());
    }

    std::reverse_iterator<Iterator> rend() {
        return std::reverse_iterator<Iterator>(begin());
    }

    std::reverse_iterator<Iterator> crbegin() {
        return std::reverse_iterator<ConstIterator>(cend());
    }

    std::reverse_iterator<Iterator> crend() {
        return std::reverse_iterator<ConstIterator>(cbegin());
    }
};

#endif
