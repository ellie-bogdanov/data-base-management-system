#pragma once

#include "common.hpp"
#include "table.hpp"

struct b_tree {
    const static int degree = 129;
    std::vector<table*> tables;
    std::vector<b_tree*> children;

    b_tree();
    ~b_tree();

    void add_child(table* table_to_add);
    table* search(size_t id);
    void split_children();
};
class database {
   private:
   public:
};