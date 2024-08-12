
#pragma once

#include <fstream>

#include "common.hpp"
#include "interpreter.hpp"
#include "serializer.hpp"

// single column in the table, it is a key in a map to a vector of entries value
enum entry_type {
    INT,
    DOUBLE,
    CHAR,
    STRING
};
struct column {
    using col_name_type = std::pair<std::string, entry_type>;
    std::string name;
    var_vec entries;

    column(const std::string& name, const var_vec& entries);
    column(const column& copy);

    column();

    // < operator for the map compatison, only compares the names
    bool operator<(const column& compare_col) const {
        return name < compare_col.name;
    }

    // == operator for comparing columns in order to perform operations only
    // compares string names
    bool operator==(const column& compare_col) const {
        return name == compare_col.name && entries == compare_col.entries;
    }

    void add_entry(const entry entry_to_add);

    entry get_entry(size_t entry_index) const;

    void print_column() const;
    /*
    static void serialize(const column& col, std::ofstream& out);
    static void deserialize(column& col, std::ifstream& in);
    */
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

// the table consists of a std::map with a column key and a vector of entries as
// it's value
class table {
private:
    // the table itself
    std::vector<column> contents;
    size_t id;
    std::string table_name;

    // pushed the whole row from contents into a provided map with the index of
    // said row

    static bool make_result_column(column& column_to_add,
        const column& compare_column_itr,
        const entry rvalue, std::string op);
    void add_matching_rows(
        table& table_to_mod,
        const std::vector<column>::const_iterator& compare_column,
        const entry rvalue, std::string op,
        std::vector<size_t>& captured_row_indicies) const;

    void set_id(size_t id);

    table(const std::vector<column>& contents, size_t id, const std::string& table_name);

    void deserialize(std::ifstream& in);
    
public:
    logger state_logger;
    //  delimiter for reading from table statements
    const static char READ_DELIM = ')';

    // delimiter for create statements seperates creation of columns
    const static char CREATE_DELIM = ',';

    // delimiter for column creation specification
    const static char COL_CREATE_DELIM = ' ';

    const static char UPDATE_DELIM = ',';
    table(const std::vector<column::col_name_type>& col_types_names, size_t pk_index,
        const std::string& table_name, size_t id);

    table(const table& copy);

    table(std::ifstream& in);

    table();

    ~table();

    size_t get_table_size();


    size_t get_table_id() const;
    std::string get_table_name() const;
    void change_pk(size_t new_pk_index);

    void copy_empty_columns(const std::vector<column>& columns);
    std::vector<column> get_contents() const;

    // reading from the table. the read statement must be of this syntax:
    // column_name comparison_operator rvalue) column_name comparison_operator
    // rvalue each end of section must be seperated by ')' example:
    // my_double_col >= 4.5) my_char_col > D
    table* read_table(std::vector<interpreter::column_comparison>& comparisons);

    void drop_row(size_t index);

    // add a specific row from a given table
    void add_row_from_table(const table& original_table, size_t row_index);

    // add a row at the bottom of the table
    void add_new_row(std::vector<entry> row);

    void print_table() const;

    void log_current_state();

    size_t get_id() const;

    void serialize(std::ofstream& out) const;
};
