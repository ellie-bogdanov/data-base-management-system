#include "../include/database.hpp"

b_tree::~b_tree() {
    for (auto single_table : tables) delete single_table;
    for (auto child : children) delete child;
}

b_tree::b_tree() {}

void b_tree::split_children() {
    b_tree* left = new b_tree();
    b_tree* right = new b_tree();
    std::for_each(
        tables.begin(), tables.begin() + degree / 2,
        [&left](auto current_table) { left->tables.push_back(current_table); });
    std::for_each(
        tables.begin() + (degree / 2) + 2,
        tables.end()[&right](auto current_table) {
            right->tables.push_back(current_table);
        });

    table* median_table = tables[(degree / 2) + 1];
    tables.clear();
    tables.push_back(median_table);
}