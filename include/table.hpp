
#pragma once

#include <string>
#include <variant>
#include <vector>

using std::size_t;
using var_vec = std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>>;
using entry = std::variant<int, double, char, std::string>;

namespace interpreter {
    // simple tokenizer that return a vector of strings which were seperated by delim in statement
    std::vector<std::string> tokenizer(const std::string &statement, char delim);

    // comparing values of two std::variant base on given comparison operator
    bool compare_values(const entry &lvalue, const entry &rvalue, std::string comp_operator);
}

// single column in the table, it is a key in a map to a vector of entries value
struct column {
    std::string name;
    var_vec entries;

    column(const std::string &name, const var_vec &entries);
    column();

    // < operator for the map compatison, only compares the names
    bool operator<(const column &compare_col) const {
        return name < compare_col.name;
    }

    // == operator for comparing columns in order to perform operations only compares string names
    bool operator==(const column &compare_col) const {
        return name == compare_col.name && entries == compare_col.entries;
    }

    void add_entry(const entry entry_to_add);

    entry get_entry(size_t entry_index) const;

    void print_column() const;
};

// the table consists of a std::map with a column key and a vector of entries as it's value
class table {
private:
    // the table itself
    std::vector<column> contents;
    column *primary_key;
    // pushed the whole row from contents into a provided map with the index of said row

    static bool make_result_column(column &column_to_add, const column &compare_column_itr, const entry rvalue, std::string op);
    void add_matching_rows(table &table_to_mod, const std::vector<column>::const_iterator &compare_column, const entry rvalue, std::string op, std::vector<size_t> &captured_row_indicies) const;

public:
    // delimiter for reading from table statements
    const static char READ_DELIM = ')';

    // delimiter for create statements seperates creation of columns
    const static char CREATE_DELIM = ',';

    // delimiter for column creation specification
    const static char COL_CREATE_DELIM = ' ';

    const static char UPDATE_DELIM = ',';
    // creating a table by providing an existing map mostly for debug purposes
    table(const std::vector<column> &contents, column *primary_key);

    // intended way of creating a table by the user with the following syntax: column_type column_name PK, column_type column_name; etc...
    // must be exactly one section that ends with PK to tell which column is the primary key
    table(const std::string &create_statement);

    table(const table &copy_table);

    table();

    ~table();

    void copy_empty_columns(const std::vector<column> &columns);
    column *const get_primary_key() const;
    std::vector<column> get_contents() const;
    int change_primary_key(column *new_key);

    // reading from the table. the read statement must be of this syntax: column_name comparison_operator rvalue) column_name comparison_operator rvalue
    // each end of section must be seperated by ')'
    // example: my_double_col >= 4.5) my_char_col > D
    table *read_table(const std::string &statement) const;

    // parse the update statement into tokens and parse them into the right type
    void update_table(const std::string &statement);

    // add a specific row from a given table
    void add_row_from_table(const table &original_table, size_t row_index);

    // add a row at the bottom of the table
    void add_new_row(std::vector<entry> row);

    void print_table() const;
};
