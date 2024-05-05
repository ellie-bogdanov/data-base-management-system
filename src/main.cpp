#include "table.hpp"
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
    ci.is_primary_key = true;
    column cd("dob_col", row_d);
    column cc("char_col", row_c);

    std::vector<column> contents{ci, cd, cc};

    std::string statement = "INT int_col PK, DOUBLE dob_col, CHAR ch_col, STR str_col";
    table t(contents);
    std::string read_statement = "int_col > 1)dob_col < 10.0)char_col == D";
    std::vector<column> result = t.read_table(read_statement);

    for (column col : result) {
        col.print_column();
    }
    // contents = t.get_contents();
    // contents[cd] = {e1d, e2d};
    //  std::cout << std::get<double>(contents.at(cd)[0].value) << '\n';

    return 0;
}
