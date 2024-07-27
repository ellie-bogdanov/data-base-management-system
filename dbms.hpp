#pragma once

#include "common.hpp"
#include "b_tree.hpp"
#include "table.hpp"

struct dbms {
	int id;
	std::string name;
	b_tree table_tree;

	dbms(int id, std::string name);
	~dbms();

	table* fetch_table(int key);
	void add_table(table& table_to_add);

};
