#pragma once
#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <stack>
#include <vector>

struct node {
    constexpr static int MAX_KEYS = 4;
    constexpr static int MIN_KEYS = 2;
    constexpr static int MAX_CHILDREN = 5;

    int current_children_size;
    int current_key_size;

    bool is_root;

    std::array<int, MAX_KEYS + 1> keys;
    std::array<node*, MAX_CHILDREN + 1> children;

    node(bool is_root);
    ~node();

    void insert_key_non_full(int key);
    node* split_children();
    void insert_child(node* child);
    node* split_root();
    int find_median_key(int key_to_add) const;
};

struct b_tree {
   private:
    // assumes first call with current_node as a leaf
    void insert_and_split(int key, node* current_node);
    // assumes first call with root node
    node* find_key_range_leaf(int key, node* current_node);

   public:
    node* root;
    std::stack<node*> path_stack;

    b_tree();
    ~b_tree();

    void insert_key(int key);

    void print_tree();
};