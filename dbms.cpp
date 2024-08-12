#include "dbms.hpp"


dbms::dbms(int id, std::string name, std::string path_to_tables) : id(id), name(name), path_to_tables(path_to_tables) {
	table_id_tree = new b_tree();
}
dbms::dbms(std::ifstream& in) {
	int id = 0;
	std::string name;
	std::string path_to_tables;
	b_tree* table_tree = new b_tree();
	serializer::deserialize(in, id, name, path_to_tables);

	table_tree->deserialize(in);

	this->id = id;
	this->name = name;
	this->path_to_tables = path_to_tables;
	this->table_id_tree = table_tree;

}
dbms::~dbms() {

	delete table_id_tree;
}


void dbms::serialize(std::ofstream & out) {
	serializer::serialize(out, id, name, path_to_tables);
	table_id_tree->serialize(out);

}

void dbms::add_table(const table& table_to_add) {
	std::ifstream in(path_to_tables, std::ios::binary | std::ios::in | std::ios::out);
	table* copy = new table(table_to_add);
	size_t table_size = copy->get_table_size();
	table_arr.push_back(copy);
	table_id_tree->insert_key({ copy->get_id(), table_size });
	size_t insert_pos = table_id_tree->calc_bytes_until_key(copy->get_id());

	in.seekg(0, std::ios::end);
	std::streampos file_size = in.tellg();

	in.seekg(insert_pos, std::ios::beg);

	std::ofstream out(path_to_tables, std::ios::binary | std::ios::in | std::ios::out);
	if (static_cast<int>(file_size) < 0) {
		copy->serialize(out);
	}
	else {
		std::vector<char> buffer(static_cast<size_t>(file_size) - insert_pos);
		in.read(buffer.data(), buffer.size());

		out.seekp(insert_pos, std::ios::beg);

		copy->serialize(out);

		out.write(buffer.data(), buffer.size());
	}

	out.close();
	in.close();

}

table* dbms::fetch_table(size_t key) {
	size_t fetch_pos = table_id_tree->calc_bytes_until_key(key);
	std::ifstream in(path_to_tables, std::ios::binary | std::ios::in | std::ios::out);
	in.seekg(fetch_pos, std::ios::beg);
	table *result = new table(in);
	table_arr.push_back(result);
	in.close();
	return result;
}
