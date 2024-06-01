#include "../include/b_tree.hpp"

node::node(int key, node* parent)
    : parent(parent), current_num_of_keys(1), current_num_of_children(0) {
    keys[0] = key;

    children.fill(nullptr);
}

node::~node() { delete[] children; }

void node::insert_non_full(int key) {
    int index = current_num_of_keys - 1;
    while (index >= 0 && keys.at(index) > key) {
        keys[index + 1] = keys[index];
        index--;
    }

    keys[index + 1] = key;
    current_num_of_keys++;
}

void node::insert_children(node* original_child, node* right_child) {
    if (current_num_of_children >= MAX_CHILDREN) {
        split_children();
    }
    int index = 0;
    while (children.at(index) != original_child) {
        index++;
    }
    if (index >= current_num_of_children) {
        throw(1);
    }

    int current_index = current_num_of_children - 1;
    while (current_index >= index + 1) {
        children[current_index + 1] = children[current_index];
        current_index--;
    }

    children[index + 1] = right_child;
    current_num_of_children++;
}

void node::split_children() {
    int median_key_index = current_num_of_keys / 2;
    int median_key = keys.at(median_key_index);

    node* right = new node(keys.at(median_key_index + 1));
    for (int i = median_key_index + 1; i < current_num_of_children; ++i) {
        right->keys[i - median_key_index - 1] = keys[i - 1];
        right->children[i - median_key_index - 1] = children[i];
    }

    right->current_num_of_keys = median_key_index;

    right->current_num_of_children = median_key_index + 1;

    current_num_of_keys -= right->current_num_of_keys;
    current_num_of_children -= right->current_num_of_children;

    for (int i = median_key_index; i < current_num_of_keys - 1; ++i) {
        keys[i] = keys[i + 1];
    }

    parent->insert_children(this, right);
    parent->insert_non_full(median_key);
}

b_tree::b_tree() { root = nullptr; }

b_tree::~b_tree() { delete root; }

void b_tree::insert(int key, node* current_node) {
    if (root == nullptr) {
        root = new node(key, nullptr);
        return;
    }

    if (current_node->current_num_of_children == 0) {
        if (current_node->current_num_of_keys >= node::MAX_KEYS) {
            current_node->split_children();
        } else {
            current_node->insert_non_full(key);
        }
        return;
    }

    int i = 0;

    while (i <= current_node->current_num_of_keys &&
           current_node->keys.at(i) > key) {
        i++;
    }
    insert(key, current_node->children[i]);
}