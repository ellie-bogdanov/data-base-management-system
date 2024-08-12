
#include <iostream>
#include <fstream>

#include "file_paths.hpp"
#include "b_tree.hpp"
#include "table.hpp"
#include "serializer.hpp"
#include "dbms.hpp"
using namespace interpreter;

int main() {
    
    std::vector<std::pair<std::string, entry_type>> col_types_1 = { {"int", INT}, {"char", CHAR}, {"double", DOUBLE}};
    size_t pk_1 = 1;
    std::vector<std::pair<std::string, entry_type>> col_types_2 = { {"string", STRING}, {"int", INT} };
    size_t pk_2 = 0;
    std::vector<std::pair<std::string, entry_type>> col_types_3 = { {"int", INT}, {"int", INT}, {"double", DOUBLE}, 
        {"char", CHAR}, {"string", STRING}, {"string", STRING}, {"int", INT} };
    size_t pk_3 = 4;

    table t1(col_types_1, pk_1, "first_table", 1);
    table t2(col_types_2, pk_2, "second_table", 2);
    table t3(col_types_3, pk_3, "third_table", 3);

    t1.add_new_row({ 'c', 2,  20.15 });
    t1.add_new_row({ 'a', 41,  1.001 });

    t2.add_new_row({ "hello world!", 100});
    t2.add_new_row({ "abcdefg aaa", 385 });

    t3.add_new_row({ "HELLO", 44, 51, 1000.20, 'D', "BYE", 12});
    t3.add_new_row({ "CPP", 21, 31, 70.3, 'v',"C++", 179 });

    std::string dbms_path = "dat\\dbms_dat\\first_db.dat";
    std::string path_to_tables = "dat\\table_dat\\first_db_tables.dat";

    dbms db(1, "first_db", path_to_tables);
    db.add_table(t1);
    db.add_table(t2);
    db.add_table(t3);

    table* t3_des = db.fetch_table(3);

 
    t3_des->print_table();
    delete t3_des;
    return 0;
}
