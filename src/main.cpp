/*
#include "../include/table.hpp"
#include <iostream>

int main() {

    int e1i(1);
    int e2i(5);

    double e1d(4.5);
    double e2d(19.34);

    char e1c('H');
    char e2c('D');

    std::vector<int> row_i = {e1i, e2i};
    std::vector<double> row_d = {e1d, e2d};
    std::vector<char> row_c = {e1c, e2c};

    column ci("int_col", row_i);
    column *primary_key = &ci;
    column cd("dob_col", row_d);
    column cc("char_col", row_c);

    std::vector<column> contents{ci, cd, cc};

    std::string create_statement = "INT int_col PK,DOUBLE dob_col,CHAR char_col,STR str_col";

    std::string update_statement1 = "6,1.3,B,hello world";
    std::string update_statement2 = "150,16502.3541541,E,bye world";
    std::string update_statement3 = "83,1287.0202,a,ldkhlasdh";
    std::string update_statement4 = "57198723,17263.3312573,l,jhnLKJH lJH KLGH KJY FV";

    table t(create_statement);
    t.update_table(update_statement1);
    t.update_table(update_statement2);
    t.update_table(update_statement3);
    t.update_table(update_statement4);
    t.drop_row(3);
    t.print_table();

    // table t(contents, primary_key);
    // t.print_table();

    std::string read_statement = "int_col >= 150)dob_col < 16000)char_col == a)str_col != hello world";
    table *result = t.read_table(read_statement);

    result->print_table();

    delete result;


    return 0;
}
*/