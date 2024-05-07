#include "../include/table.hpp"
#include <algorithm>
#include <execution>
#include <iostream>
#include <iterator>
#include <sstream>

using std::size_t;

column::column(const std::string &name, const std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>> &entries) : name(name), entries(entries) {
}

column::column() {
    name = "NULL";
}

void column::add_entry(const std::variant<int, double, char, std::string> entry_to_add) {

    entries = std::visit([entry_to_add]<class T>(std::vector<T> &vec) -> std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>> {
        if (std::holds_alternative<int>(entry_to_add)) {
            vec.push_back(std::get<T>(entry_to_add));
            return vec;
        } else if (std::holds_alternative<double>(entry_to_add)) {
            vec.push_back(std::get<T>(entry_to_add));
            return vec;
        } else if (std::holds_alternative<char>(entry_to_add)) {
            vec.push_back(std::get<T>(entry_to_add));
            return vec;
        } else if (std::holds_alternative<std::string>(entry_to_add)) {
            vec.push_back(std::get<T>(entry_to_add));
            return vec;
        }
        std::cout << "entry with undefined type \n";
        throw(1);
    },
                         entries);
}

entry column::get_entry(size_t entry_index) const {
    return std::visit([entry_index]<class T>(const std::vector<T> &entries) -> entry { return entries[entry_index]; }, entries);
}

table::table(const std::vector<column> &contents, column *primary_key) : contents(contents) {
    this->primary_key = new column(primary_key->name, primary_key->entries);
}

table::table(const table &copy_empty) {
    for (const column &copy_col : copy_empty.contents) {
        std::visit([this, copy_col]<class T>(const std::vector<T> &entries) {
            if (std::get_if<std::vector<T>>(&copy_col.entries)) {
                contents.emplace_back(copy_col.name, std::vector<T>());
            }
        },
                   copy_col.entries);
        if (copy_empty.primary_key == &copy_col)
            primary_key = &contents.back();
    }
}

table::~table() {
    delete primary_key;
}

table::table(const std::string &create_statement) {
    std::vector<column> contents;
    // creating a std::stringstream of provided statement for parsing
    std::stringstream create_statement_stream(create_statement);

    // a flag to check wether a primary_key was found or not
    bool is_key_found = false;
    std::string token;

    while (std::getline(create_statement_stream, token, CREATE_DELIM)) {

        // column specifiers seperated into different strings in params. size of params should be either 2 or 3
        std::vector<std::string> params;

        // another stream for creating a column this time
        std::stringstream token_stream(token);

        // current specifier in the column creation
        std::string param;

        while (std::getline(token_stream, param, COL_CREATE_DELIM)) { // each group divide by space
            params.push_back(param);
        }

        // invalid column create syntax
        if ((params.size() > 3 || params.size() < 2) || (params.size() == 3 && is_key_found)) {
            switch (params.size()) {
            case 3:
                std::cout << "key has been specified more than once \n";
                break;
            default:
                std::cout << "invalid amount of specifiers should be 2 for regular column or 3 for PK column \n";
                break;
            }
            throw(1);
        }

        // the name of the column should be second after the type name
        std::string col_name = params[1];
        std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>> entries;
        if (params[0] == "INT") {
            entries = std::vector<int>();
        } else if (params[0] == "DOUBLE") {
            entries = std::vector<double>();
        } else if (params[0] == "CHAR") {
            entries = std::vector<char>();
        } else if (params[0] == "STR") {
            entries = std::vector<std::string>();
        } else {
            std::cout << "type " << params[0] << " not supported.\n";
            throw(1);
        }

        // create the column with the specifiers
        column current_col(col_name, entries);

        // check if current group is a primary key
        if (params.size() == 3) {
            if (params[2] != "PK") {
                std::cout << "wrong third param.\n";
                throw(1);
            }

            // the key is now found so set flag to true. only one PK allowed
            is_key_found = true;

            // set the boolean value of is_primary_key of column to true to indicate that this is the PK
            primary_key = new column(col_name, entries);
        }

        contents.push_back(current_col);
    }

    // throw an error in case of user not specifying which column is the PK
    if (!is_key_found) {
        std::cout << "no primary key provided.\n";
        throw(1);
    }
}

std::vector<column> table::get_contents() const {
    return contents;
}

std::vector<std::string> interpreter::tokenizer(const std::string &statement, char delim) {
    std::vector<std::string> tokens;
    std::stringstream token_stream(statement);
    std::string current_token;
    while (std::getline(token_stream, current_token, delim)) {
        tokens.push_back(current_token);
    }

    return tokens;
}

// checking all options of comparisons and
bool interpreter::compare_values(const std::variant<int, double, char, std::string> &lvalue, const std::variant<int, double, char, std::string> &rvalue, std::string comp_operator) {
    bool is_valid_comp_op = true;
    bool compare_result = (comp_operator == "==") ? (lvalue == rvalue) : (comp_operator == ">") ? (lvalue > rvalue)
                                                                     : (comp_operator == "<")   ? (lvalue < rvalue)
                                                                     : (comp_operator == "<=")  ? (lvalue <= rvalue)
                                                                     : (comp_operator == ">=")  ? (lvalue >= rvalue)
                                                                                                : is_valid_comp_op = false;

    if (!is_valid_comp_op) {
        std::cout << "invalid comparison operator '" << comp_operator << "' \n";
        throw(1);
    }

    return compare_result;
}

bool table::make_result_column(column &column_to_add, const column &compare_column_itr, const std::variant<int, double, char, std::string> rvalue, std::string op) {
    bool is_empty_flag = true;
    std::visit([rvalue, op, &column_to_add, &is_empty_flag]<class T>(const std::vector<T> &compare_column_entries) {
        for (const T &current_entry : compare_column_entries) {
            if (interpreter::compare_values(current_entry, rvalue, op)) {
                column_to_add.add_entry(current_entry);
                is_empty_flag = false;
            }
        }
    },
               compare_column_itr.entries);

    return is_empty_flag;
}

void table::add_matching_rows(table &table_to_mod, const std::vector<column>::const_iterator &compare_column, const entry rvalue, std::string op, std::vector<size_t> &captured_row_indicies) const {

    std::visit([&table_to_mod, rvalue, op, &captured_row_indicies, this]<class T>(const std::vector<T> &compare_column) {
        for (size_t i = 0; i < compare_column.size(); ++i) {
            if (std::find(captured_row_indicies.begin(), captured_row_indicies.end(), i) != captured_row_indicies.end()) {
                continue;
            }
            if (interpreter::compare_values(compare_column[i], rvalue, op)) {
                captured_row_indicies.push_back(i);
                table_to_mod.add_row(*this, i);
            }
        }
    },
               compare_column->entries);
}
table *table::read_table(const std::string &statement) const {

    table *ptr_result_table = new table(*this);
    std::vector<std::string> tokens = interpreter::tokenizer(statement, table::READ_DELIM);
    std::vector<size_t> captured_row_indicies;

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::stringstream token_stream(tokens[i]);

        std::string col_name;
        std::string op;
        std::string rvalue;

        // spliting into 3 components (lvalue) (< or < or == or >= or <=) (rvalue)
        std::getline(token_stream, col_name, ' ');
        std::getline(token_stream, op, ' ');
        std::getline(token_stream, rvalue, ' ');

        std::vector<column>::const_iterator current_col = contents.begin();
        while (current_col != contents.end() && current_col->name != col_name)
            ++current_col;

        // if didn't find then it is an invalid column name and throw error
        if (current_col == contents.end()) {
            std::cout << "invalid column name: " << col_name << '\n';
            throw(1);
        }

        // creating a variant to compare the rvalue to the values inside the column
        std::variant<int, double, char, std::string> rvalue_converted;
        column col_to_add;
        col_to_add.name = current_col->name;

        // using the right conversion based on type extracted'
        if (std::holds_alternative<std::vector<int>>(current_col->entries)) {
            rvalue_converted = std::stoi(rvalue);
            col_to_add.entries = std::vector<int>();
        } else if (std::holds_alternative<std::vector<double>>(current_col->entries)) {
            rvalue_converted = std::stod(rvalue);
            col_to_add.entries = std::vector<double>();
        } else if (std::holds_alternative<std::vector<char>>(current_col->entries)) {
            rvalue_converted = rvalue[0];
            col_to_add.entries = std::vector<char>();
        } else if (std::holds_alternative<std::vector<std::string>>(current_col->entries)) {
            rvalue_converted = rvalue;
            col_to_add.entries = std::vector<std::string>();
        } else {
            std::cout << "column \'" << current_col->name << "\' holds invalid type \n";
            throw(1);
        }

        add_matching_rows(*ptr_result_table, current_col, rvalue_converted, op, captured_row_indicies);
    }
    return ptr_result_table;
}

int table::change_primary_key(column *primary_key) {
    if (primary_key == nullptr)
        return 1;
    this->primary_key = primary_key;
    return 0;
}

void table::print_table() const {
    for (const column &col : contents) {
        std::cout << col.name << ": ";
        col.print_column();
    }
}

void column::print_column() const {
    std::visit([]<class T>(const std::vector<T> &entries_to_print) {
        if (entries_to_print.size() > 0) {
            std::copy(entries_to_print.begin(), entries_to_print.end() - 1, std::ostream_iterator<T>(std::cout, ", "));
            std::cout << entries_to_print.back() << std::endl;
        }
    },
               entries);
}

void table::add_row(const table &original_table, size_t row_index) {
    for (size_t i = 0; i < original_table.get_contents().size(); ++i) {
        contents[i].add_entry(original_table.get_contents()[i].get_entry(row_index));
    }
}