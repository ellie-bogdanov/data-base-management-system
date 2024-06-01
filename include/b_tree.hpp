#pragma once
#include <array>

#include "common.hpp"

struct node {
    constexpr static size_t MIN_CHILDREN = 2;  // the degree also known as 't'
    constexpr static size_t MIN_KEYS = MIN_CHILDREN - 1;
    constexpr static size_t MAX_KEYS = MIN_CHILDREN * 2 - 1;
    constexpr static size_t MAX_CHILDREN = MAX_KEYS + 1;
    size_t current_num_of_keys;
    size_t current_num_of_children;
    std::array<node*, MAX_CHILDREN> children;
    std::array<int, MAX_KEYS> keys;
    node* parent;
    node(int key, node* parent);
    ~node();
    void insert_non_full(int key);
    void split_children(node* parent);

   private:
    void insert_children(node* original_child, node* right_child);
};

struct b_tree {
    node* root;

    b_tree();
    ~b_tree();
    void insert(int key, node* current_node, node* parent);
};