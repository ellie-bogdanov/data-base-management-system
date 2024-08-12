#pragma once

#include "common.hpp"

#include <fstream>
#include <queue>
#include <stack>

using key_size_pair = std::pair<size_t, size_t>;

struct node {
    constexpr static size_t MAX_KEYS = 4;
    constexpr static size_t MIN_KEYS = 2;
    constexpr static size_t MAX_CHILDREN = 5;

    size_t current_children_size;
    size_t current_key_size;

    bool is_root;


    std::array<key_size_pair, MAX_KEYS + 1> keys;
    std::array<node*, MAX_CHILDREN + 1> children;

    node(bool is_root);
    node();
 
    ~node();

    void insert_key_non_full(size_t key, size_t table_size);
    node* split_children();
    void insert_child(node* child);
    node* split_root();
    key_size_pair find_median_key(size_t key_to_add) const;


    
    void serialize(std::ofstream& out);
    void deserialize(std::ifstream& in);
    

};

struct b_tree {
private:
    // assumes first call with current_node as a leaf
    void insert_and_split(key_size_pair key, node* current_node);
    // assumes first call with root node
    node* find_key_range_leaf(size_t key, node* current_node);
    size_t calc_bytes_until_key(size_t key, node* current_node, size_t path_size = 0);

public:
    std::string name;
    node* root;
    std::stack<node*> path_stack;

    b_tree(std::string name);
    b_tree();
    ~b_tree();

    void insert_key(key_size_pair key);

    void print_tree();

    size_t calc_bytes_until_key(size_t key);
    
    void serialize(std::ofstream& out);
    void deserialize(std::ifstream& in);
    
};