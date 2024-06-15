#include "../include/b_tree.hpp"

node::node(bool is_root) : is_root(is_root) {
    keys = {};
    current_key_size = 0;
    current_children_size = 0;
}

node::~node() {
    for (int i = 0; i < current_children_size; ++i) delete children[i];
}

b_tree::b_tree() { root = nullptr; }
b_tree::~b_tree() { delete root; }

int node::find_median_key(int key_to_add) const {
    int median_key = keys[(current_key_size / 2) - 1];
    if (key_to_add > median_key && key_to_add < keys[current_key_size / 2])
        median_key = key_to_add;
    else {
        median_key = keys[current_key_size / 2];
    }
    return median_key;
}

void node::insert_key_non_full(int key) {
    int i = current_key_size - 1;
    while (i >= 0 && key < keys[i]) {
        keys[i + 1] = keys[i];
        --i;
    }

    keys[i + 1] = key;
    current_key_size++;
}

node* node::split_children() {
    std::array<int, MAX_KEYS + 1> less_than_key = {};
    std::array<int, MAX_KEYS + 1> more_than_key = {};
    int median_key = keys[node::MAX_KEYS / 2];
    std::partition_copy(
        keys.begin(), keys.begin() + current_key_size, less_than_key.begin(),
        more_than_key.begin(),
        [median_key](int current_key) { return current_key <= median_key; });

    keys = less_than_key;
    current_key_size = (node::MAX_KEYS / 2);
    current_children_size /= 2;

    node* more_node = new node(false);
    more_node->keys = more_than_key;
    more_node->current_key_size = (node::MAX_KEYS / 2);
    more_node->current_children_size = current_children_size;
    for (int i = 0; i < more_node->current_children_size; ++i) {
        more_node->children[i] = children[i + more_node->current_children_size];
    }

    return more_node;
}

void node::insert_child(node* child) {
    int i = current_children_size;
    while (i > 0 && child->keys[0] < keys[0]) {
        children[i] = children[i - 1];
        --i;
    }
    children[i] = child;
    ++current_children_size;
}

node* node::split_root() {
    if (!is_root) {
        return nullptr;
    }

    node* new_root = new node(true);
    new_root->insert_child(this->split_children());
    new_root->keys[0] = this->keys[this->current_key_size];
    new_root->insert_child(this);
    new_root->current_key_size = 1;

    is_root = false;
    new_root->is_root = true;
    new_root->current_children_size = 2;

    return new_root;
}

void b_tree::insert_and_split(int key, node* current_node) {
    current_node->insert_key_non_full(key);
    if (current_node->current_key_size > node::MAX_KEYS) {
        if (current_node->is_root) {
            root = root->split_root();
            return;
        }
        path_stack.pop();
        path_stack.top()->insert_child(current_node->split_children());
        insert_and_split(current_node->keys[current_node->current_key_size],
                         path_stack.top());
    }
}

node* b_tree::find_key_range_leaf(int key, node* current_node) {
    path_stack.push(current_node);
    if (current_node->current_children_size == 0) {
        return current_node;
    }

    int i = 0;
    while (i < current_node->current_key_size && key > current_node->keys[i])
        ++i;

    return find_key_range_leaf(key, current_node->children[i]);
}

void b_tree::insert_key(int key) {
    if (root == nullptr) {
        root = new node(true);
    }
    find_key_range_leaf(key, root);
    insert_and_split(key, path_stack.top());
    while (!path_stack.empty()) path_stack.pop();
}

void b_tree::print_tree() {
    std::vector<node*> tree_vec;
    if (root == nullptr) return;

    tree_vec.push_back(root);

    while (!tree_vec.empty()) {
        std::vector<node*> next_level;
        for (int i = 0; i < tree_vec.size(); ++i) {
            for (int j = 0; j < tree_vec[i]->current_children_size; ++j)
                next_level.push_back(tree_vec[i]->children[j]);
        }

        for (int i = 0; i < tree_vec.size(); ++i) {
            for (int j = 0; j < tree_vec[i]->current_key_size; ++j)
                std::cout << tree_vec[i]->keys[j] << ", ";
        }
        std::cout << '\n';

        tree_vec = next_level;
    }
}
