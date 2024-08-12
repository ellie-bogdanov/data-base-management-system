#include "b_tree.hpp"
#include <cassert>

node::node(bool is_root) : is_root(is_root) {
    keys = {};
    children = {};
    current_key_size = 0;
    current_children_size = 0;
}

node::node() {
    keys = {};
    children = {};
    current_key_size = 0;
    current_children_size = 0;
    is_root = false;
}

node::~node() {
    for (int i = 0; i < current_children_size; ++i) delete children[i];
}

b_tree::b_tree(std::string name) : name(name) { root = nullptr; }
b_tree::b_tree() {
    name = " ";
    root = nullptr;
}
b_tree::~b_tree() { delete root; }

key_size_pair node::find_median_key(size_t key_to_add) const {
    key_size_pair median_key = keys[(current_key_size / 2) - 1];
    if (key_to_add > median_key.first && key_to_add < keys[current_key_size / 2].first)
        median_key.first = key_to_add;
    else {
        median_key = keys[current_key_size / 2];
    }
    return median_key;
}

void node::insert_key_non_full(size_t key, size_t table_size) {
    int i = current_key_size - 1;
    while (i >= 0 && key < keys[i].first) {
        keys[i + 1] = keys[i];
        --i;
    }

    keys[i + 1] = {key, table_size};
    current_key_size++;
}

node* node::split_children() {
    std::array<key_size_pair, MAX_KEYS + 1> less_than_key = {};
    std::array<key_size_pair, MAX_KEYS + 1> more_than_key = {};
    key_size_pair median_key = keys[node::MAX_KEYS / 2];
    std::partition_copy(
        keys.begin(), keys.begin() + current_key_size, less_than_key.begin(),
        more_than_key.begin(),
        [median_key](key_size_pair current_key) { return current_key.first <= median_key.first; });

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

void b_tree::insert_and_split(key_size_pair key, node* current_node) {
    current_node->insert_key_non_full(key.first, key.second);
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

node* b_tree::find_key_range_leaf(size_t key, node* current_node) {
    path_stack.push(current_node);
    if (current_node->current_children_size == 0) {
        return current_node;
    }

    size_t i = 0;
    while (i < current_node->current_key_size && key > current_node->keys[i].first)
        ++i;

    assert(current_node->current_key_size >= 0 && current_node->current_key_size <= 5);

    return find_key_range_leaf(key, current_node->children[i]);

}

void b_tree::insert_key(key_size_pair key) {
    if (root == nullptr) {
        root = new node(true);
    }
    find_key_range_leaf(key.first, root);
    insert_and_split(key, path_stack.top());
    while (!path_stack.empty()) path_stack.pop();
}

size_t b_tree::calc_bytes_until_key(size_t key) {

    return calc_bytes_until_key(key, root);
         
}

size_t b_tree::calc_bytes_until_key(size_t key, node* current_node, size_t path_size) {
    size_t i = 0;

    while (i < node::MAX_KEYS - 1 && i < current_node->current_key_size && key > current_node->keys[i].first) {
        path_size += current_node->keys[i].second;
        ++i;
    }



    if (key == current_node->keys[i].first)
        return path_size;


    if (current_node->current_children_size == 0)
        return 0;

    while (i < node::MAX_KEYS - 1 && i < current_node->current_key_size) {
        path_size += current_node->keys[i].second;
        ++i;
    }

    path_size += current_node->keys[i].second;




    return this->calc_bytes_until_key(key, current_node->children[i], path_size);


    

}

void b_tree::print_tree() {
    std::vector<node*> tree_vec;
    if (root == nullptr) return;

    tree_vec.push_back(root);

    while (!tree_vec.empty()) {
        std::vector<node*> next_level;
        for (size_t i = 0; i < tree_vec.size(); ++i) {
            for (size_t j = 0; j < tree_vec[i]->current_children_size; ++j)
                next_level.push_back(tree_vec[i]->children[j]);
        }

        for (size_t i = 0; i < tree_vec.size(); ++i) {
            for (size_t j = 0; j < tree_vec[i]->current_key_size; ++j)
                std::cout << tree_vec[i]->keys[j].first << ", ";
        }
        std::cout << '\n';

        tree_vec = next_level;
    }
}


void b_tree::serialize(std::ofstream& out) {
    if (root == nullptr)
        return;
    
    size_t name_len = name.size();
    out.write(reinterpret_cast<char*>(&name_len), sizeof(name_len));
    out.write(name.c_str(), name_len);

    root->serialize(out);
    

}


void node::serialize(std::ofstream& out) {
    
    out.write(reinterpret_cast<char*>(&current_key_size), sizeof(current_key_size));
    for (size_t i = 0; i < current_key_size; ++i) {
        out.write(reinterpret_cast<char*>(&keys[i].first), sizeof(keys[i].first));
        out.write(reinterpret_cast<char*>(&keys[i].second), sizeof(keys[i].second));
    }
    out.write(reinterpret_cast<char*>(&current_children_size), sizeof(current_children_size));
    for (size_t i = 0; i < current_children_size && children[i]; ++i)
        children[i]->serialize(out);
 
}

void b_tree::deserialize(std::ifstream& in) {
    size_t name_len = 0;
    std::string name;
    node* root = new node(true);
    size_t root_key_size = 0;
    size_t root_children_size = 0;

    in.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
    name.resize(name_len);
    in.read(&name[0], name_len);

    root->deserialize(in);

    this->name = name;
    this->root = root;
}

void node::deserialize(std::ifstream & in) {
    size_t current_key_size = 0;
    size_t current_children_size = 0;
    std::array<key_size_pair, MAX_KEYS + 1> keys = {};
    std::array<node*, MAX_CHILDREN + 1> children = {};

    in.read(reinterpret_cast<char*>(&current_key_size), sizeof(current_key_size));

    for (size_t i = 0; i < current_key_size; ++i) {
        key_size_pair current_key;
        in.read(reinterpret_cast<char*>(&current_key.first), sizeof(current_key.first));
        in.read(reinterpret_cast<char*>(&current_key.second), sizeof(current_key.second));
        keys[i] = current_key;
    }

    in.read(reinterpret_cast<char*>(&current_children_size), sizeof(current_children_size));

    for (size_t i = 0; i < current_children_size; ++i) {
        children[i] = new node(false);
        children[i]->deserialize(in);
    }

    this->current_key_size = current_key_size;
    this->current_children_size = current_children_size;
    this->keys = keys;
    this->children = children;
}

/*
void node::serialize(std::ofstream& out) {

}
void deserialize(std::ifstream& in) {

}
*/