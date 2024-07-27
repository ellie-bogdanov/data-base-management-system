#include "dbms.hpp"

dbms::dbms(int id, std::string name) : id(id), name(name) {
	table_tree = b_tree::deserialize("b_tree" + name);
}

dbms::~dbms() {
	std::ofstream out(file_paths::DBMS_DAT_FILE_LOCATION + "b_tree" + name + ".dat", std::ios::binary);
	b_tree::serialize(table_tree, out);
	out.close();
}


table* dbms::fetch_table(int key) {
	return nullptr;
}


void dbms::add_table(table& table_to_add) {
	std::ofstream out(file_paths::DBMS_DAT_FILE_LOCATION + name + ".dat", std::ios::binary);
	table::serialize(table_to_add, out);
	table_tree.insert_key(table_to_add.get_id());
	out.close();
}