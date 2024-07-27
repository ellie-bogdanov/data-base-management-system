#include "b_tree.hpp"

node::node(bool is_root) : is_root(is_root) {
    keys = {};
    children = {};
    current_key_size = 0;
    current_children_size = 0;
}

node::~node() {
    for (int i = 0; i < current_children_size; ++i) delete children[i];
}

b_tree::b_tree(std::string name) : name(name) { 
    root = nullptr;
    amount_of_keys = 0;
}

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

void node::insert_key_non_full(int key, int offset) {

    int i = current_key_size - 1;
    while (i >= 0 && key < keys[i]) {
        keys[i + 1] = keys[i];
        offsets[i + 1] = offsets[i];
        --i;
    }

    keys[i + 1] = key;
    offsets[i + 1] = offset;
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
    for (int i = 0; i < more_node->current_key_size; ++i) {
        more_node->offsets[i] = offsets[i + more_node->current_key_size + 1];
    }

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
    new_root->offsets[0] = this->offsets[this->current_key_size];
    new_root->insert_child(this);
    new_root->current_key_size = 1;

    is_root = false;
    new_root->is_root = true;
    new_root->current_children_size = 2;

    return new_root;
}

void b_tree::insert_and_split(int key, int offset, node* current_node) {
    current_node->insert_key_non_full(key, offset);
    if (current_node->current_key_size > node::MAX_KEYS) {
        if (current_node->is_root) {
            root = root->split_root();
            return;
        }
        path_stack.pop();
        path_stack.top()->insert_child(current_node->split_children());
        insert_and_split(current_node->keys[current_node->current_key_size], current_node->offsets[current_node->current_key_size],
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
    amount_of_keys++;
    find_key_range_leaf(key, root);
    insert_and_split(key, amount_of_keys, path_stack.top());
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
            for (int j = 0; j < tree_vec[i]->current_key_size; ++j) {
                std::cout << tree_vec[i]->keys[j] << ": ";
                std::cout << tree_vec[i]->offsets[j] << ", ";
            }

        }
        std::cout << '\n';

        tree_vec = next_level;
    }
}

void node::serialize(node* ser_node, std::ofstream& out) {
    out.write(reinterpret_cast<char*>(&ser_node->current_key_size),
        sizeof(ser_node->current_key_size));
    out.write(reinterpret_cast<char*>(&ser_node->current_children_size),
        sizeof(ser_node->current_children_size));
    out.write(reinterpret_cast<char*>(&ser_node->is_root),
        sizeof(ser_node->is_root));

    for (int i = 0; i < ser_node->current_key_size; ++i) {
        out.write(reinterpret_cast<char*>(&ser_node->keys[i]),
            sizeof(ser_node->keys[i]));
 
        out.write(reinterpret_cast<char*>(&ser_node->offsets[i]),
            sizeof(ser_node->offsets[i]));
    }

    if (ser_node->current_children_size == 0) {
        return;
    }
    else {
        for (int i = 0; i < ser_node->current_children_size; ++i) {
            node::serialize(ser_node->children[i], out);
        }
    }
}
void node::deserialize(node*& des_node, std::ifstream& in) {
    int current_key_size = 0;
    in.read(reinterpret_cast<char*>(&current_key_size),
        sizeof(current_key_size));

    int current_children_size = 0;
    in.read(reinterpret_cast<char*>(&current_children_size),
        sizeof(current_children_size));

    bool is_root = false;
    in.read(reinterpret_cast<char*>(&is_root), sizeof(is_root));

    std::array<int, node::MAX_KEYS + 1> keys = {};
    std::array<int, node::MAX_KEYS + 1> offsets = {};
    for (int i = 0; i < current_key_size; ++i) {
        int key = 0;
        int offset = 0;
        in.read(reinterpret_cast<char*>(&key), sizeof(key));
        in.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        
        keys[i] = key;
        offsets[i] = offset;
    }

    des_node = new node(is_root);
    des_node->keys = keys;
    des_node->offsets = offsets;
    des_node->current_key_size = current_key_size;
    des_node->current_children_size = current_children_size;
    for (int i = 0; i < current_children_size; ++i) {
        node::deserialize(des_node->children[i], in);
    }
}

void b_tree::serialize(b_tree& tree, std::ofstream &out) {
    size_t name_len = tree.name.size();
    out.write(reinterpret_cast<char*>(&name_len), sizeof(name_len));

    out.write(tree.name.c_str(), name_len);

    out.write(reinterpret_cast<char*>(&tree.amount_of_keys), sizeof(tree.amount_of_keys));

    node::serialize(tree.root, out);
}
b_tree b_tree::deserialize(std::string file_name) {
    std::ifstream in(file_paths::DBMS_DAT_FILE_LOCATION + file_name + ".dat", std::ios::binary);
    size_t name_len = 0;
    in.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

    std::string name;
    name.resize(name_len);
    in.read(&name[0], name_len);

    int amount_of_keys = 0;
    in.read(reinterpret_cast<char*>(&amount_of_keys), sizeof(amount_of_keys));
    b_tree tree(name);
    tree.amount_of_keys = amount_of_keys;

   

    node::deserialize(tree.root, in);
    in.close();

    return tree;
}

b_tree::b_tree() {
    name = "";
    root = nullptr;
    amount_of_keys = 0;
}