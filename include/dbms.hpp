#pragma once

#include "common.hpp"
#include "table.hpp"

class dbms {
   private:
    std::unordered_map<std::string, table *> tables;

   public:
    dbms();
    dbms(const std::vector<table *> &tables);
    ~dbms();

    table *const get_table(const std::string &name) const;
    void add_table(table *table_to_add);
    table drop_table(const std::string &table_name);
};