
#pragma once

#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

namespace interpreter {
    // simple tokenizer that return a vector of strings which were seperated by delim in statement
    std::vector<std::string> tokenizer(const std::string &statement, char delim);

    // comparing values of two std::variant base on given comparison operator
    bool compare_values(const std::variant<int, double, char, std::string> &lvalue, const std::variant<int, double, char, std::string> &rvalue, std::string comp_operator);
}

// currently available value types in the table. NILL means there is no value inside or a std::string equals to NILL
enum types {
    INT,
    DOUBLE,
    CHAR,
    STR,
    NILL
};

// single entry in the table, each entry is part of a vector which is the value of a column key in the table map
struct entry {
    std::variant<int, double, char, std::string> value;
    entry(std::variant<int, double, char, std::string> value);
};

// single column in the table, it is a key in a map to a vector of entries value
struct column {
    std::string name;
    std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>> entries;
    bool is_primary_key = false;

    column(const std::string &name, const std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>> &entries);
    column();

    // < operator for the map compatison, only compares the names
    bool operator<(const column &compare_col) const {
        return name < compare_col.name;
    }

    // == operator for comparing columns in order to perform operations only compares string names
    bool operator==(const column &compare_col) const {
        return name == compare_col.name && entries == compare_col.entries;
    }

    void add_entry(const std::variant<int, double, char, std::string> entry_to_add);

    void print_column() const;
};

// the table consists of a std::map with a column key and a vector of entries as it's value
class table {
private:
    // the table itself
    std::vector<column> contents;
    column *primary_key;
    // pushed the whole row from contents into a provided map with the index of said row

    static void make_result_column(column &column_to_add, const column &compare_column_itr, const std::variant<int, double, char, std::string> rvalue, std::string op);

public:
    // delimiter for reading from table statements
    const static char READ_DELIM = ')';

    // delimiter for create statements seperates creation of columns
    const static char CREATE_DELIM = ',';

    // delimiter for column creation specification
    const static char COL_CREATE_DELIM = ' ';

    // creating a table by providing an existing map mostly for debug purposes
    table(const std::vector<column> &contents);

    // intended way of creating a table by the user with the following syntax: column_type column_name PK, column_type column_name; etc...
    // must be exactly one section that ends with PK to tell which column is the primary key
    table(const std::string &create_statement);

    std::string get_primary_key() const;
    std::vector<column> get_contents() const;
    int change_primary_key(const column &new_key);

    // reading from the table. the read statement must be of this syntax: column_name comparison_operator rvalue) column_name comparison_operator rvalue
    // each end of section must be seperated by ')'
    // example my_double_col >= 4.5) my_char_col > D
    std::vector<column> read_table(const std::string &statement) const;

    void print_table() const;
};
