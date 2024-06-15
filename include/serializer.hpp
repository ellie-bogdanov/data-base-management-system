#pragma once
#include <fstream>

#include "b_tree.hpp"
#include "table.hpp"

struct serializer {
    void serialize_column(std::ofstream& out, const column& col);
    void deserialize_column(std::ifstream& in, column& col);

    void serialize_table(table* table_to_serialize);
};