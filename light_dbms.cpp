
#include <iostream>

#include "b_tree.hpp"
#include "table.hpp"
int main() {
    int e1i(1);
    int e2i(5);

    double e1d(4.5);
    double e2d(19.34);

    char e1c('H');
    char e2c('D');

    std::vector<int> row_i = { e1i, e2i };
    std::vector<double> row_d = { e1d, e2d };
    std::vector<char> row_c = { e1c, e2c };
    std::vector<std::string> col_str = { "hello", "world", "i'm", "serializing" };

    column ci("int_col", row_i);
    column* primary_key = &ci;
    column cd("dob_col", row_d);
    column cc("char_col", row_c);
    column cs("str_col", col_str);

    std::vector<column> contents{ ci, cd, cc };

    std::string create_statement =
        "INT int_col PK,DOUBLE dob_col,CHAR char_col,STR str_col";

    std::string update_statement1 = "6,1.3,B,hello world";
    std::string update_statement2 = "150,16502.3541541,E,bye world";
    std::string update_statement3 = "83,1287.0202,a,ldkhlasdh";
    std::string update_statement4 =
        "57198723,17263.3312573,l,jhnLKJH lJH KLGH KJY FV";

    table t(create_statement, "first_table", 1);
    t.update_table(update_statement1);
    t.update_table(update_statement2);
    t.update_table(update_statement3);
    t.update_table(update_statement4);
    t.drop_row(3);
    std::cout << "before:\n";

    t.print_table();

    table::serialize(t);
    table* p_table = table::deserialize(t.get_table_name());
    std::cout << "after:\n";
    p_table->print_table();

    
    std::string read_statement =
        "int_col >= 150)dob_col < 16000)char_col == a)str_col != hello world";
         table *result = t.read_table(read_statement);


    result->print_table();

    t.log_current_state();
    
    
    b_tree tree("cool_b_tree");

    std::string table_name1 = "first_table";
    std::string table_name2 = "second_table";
    std::string table_name3 = "third_table";
    std::string table_name4 = "fourth_table";
    std::string table_name5 = "fifth_table";
    std::string table_name6 = "sixth_table";
    std::string table_name7 = "seventh_table";

    tree.insert_key(1, table_name1);
    tree.insert_key(2, table_name2);
    tree.insert_key(3, table_name3);
    tree.insert_key(4, table_name4);
    tree.insert_key(5, table_name5);
    tree.insert_key(6, table_name6);
    tree.insert_key(7, table_name7);
    
    b_tree::serialize(tree);
    b_tree des_tree = b_tree::deserialize(tree.name);
    des_tree.print_tree();

    return 0;
}
