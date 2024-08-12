#pragma once

#include "common.hpp"
#include "b_tree.hpp"
#include "table.hpp"

#include <fstream>

struct dbms {
	int id;
	std::string name;
	b_tree *table_id_tree;
	std::vector<table*> table_arr;
	std::string path_to_tables;

	dbms(int id, std::string name, std::string path_to_tables);
	dbms(std::ifstream& in);
	~dbms();

	void serialize(std::ofstream& out);

	table* fetch_table(size_t key);
	void add_table(const table &table_to_add);

};
