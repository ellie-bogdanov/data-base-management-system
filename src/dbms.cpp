#include "../include/dbms.hpp"

dbms::dbms()
{
}

dbms::dbms(const std::vector<table *> &tables)
{
    for (table *t : tables)
    {
        this->tables.insert({t->get_table_name(), t});
    }
}

dbms::~dbms()
{
}

table *const dbms::get_table(const std::string &name) const
{
    return tables.at(name);
}

void dbms::add_table(table *table_to_add)
{
    tables[table_to_add->get_table_name()] = table_to_add;
}

table dbms::drop_table(const std::string &table_name)
{
    table copy(*tables.at(table_name));
    tables.erase(table_name);
    return copy;
}