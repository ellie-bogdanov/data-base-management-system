#include "table.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <execution>
#include <iostream>
#include <iterator>
#include <sstream>

using std::size_t;

column::column(const std::string& name, const var_vec& entries)
    : name(name), entries(entries) {}

column::column() { name = "NULL"; }

void column::add_entry(const entry entry_to_add) {
    // visits the active vector type of entries to add the requested entry
    entries = std::visit(
        [entry_to_add]<class T>(std::vector<T> &vec) -> var_vec {
        vec.push_back(std::get<T>(entry_to_add));
        return vec;
    },
        entries);
}

entry column::get_entry(size_t entry_index) const {
    // similar to column::add_entry but just returns the entry at specified
    // index
    return std::visit(
        [entry_index]<class T>(const std::vector<T> &entries) -> entry {
        return entries[entry_index];
    },
        entries);
}

table::table(const std::vector<column>& contents, column* primary_key,
    size_t id, const std::string& table_name)
    : contents(contents),
    primary_key(primary_key),
    id(id),
    table_name(table_name) {}

table::table(const table& copy_table, const std::string& table_name, size_t id)
    : table_name(table_name), id(id) {
    primary_key = new column(copy_table.primary_key->name,
        copy_table.primary_key->entries);
    contents = copy_table.contents;
}

table::table() {
    primary_key = nullptr;
    table_name = "temp_table";
}

void table::set_id(size_t id) { this->id = id; }

table::~table() { delete primary_key; }

table::table(const std::string& create_statement, const std::string& table_name,
    size_t id)
    : table_name(table_name), id(id) {
    state_logger = logger(table_name);
    primary_key = nullptr;
    // std::vector<column> contents;
    // creating a std::stringstream of provided statement for parsing
    std::stringstream create_statement_stream(create_statement);

    // a flag to check wether a primary_key was found or not
    bool is_key_found = false;
    std::string token;

    while (std::getline(create_statement_stream, token, CREATE_DELIM)) {
        // column specifiers seperated into different strings in params. size of
        // params should be either 2 or 3
        std::vector<std::string> params;

        // another stream for creating a column this time
        std::stringstream token_stream(token);

        // current specifier in the column creation
        std::string param;

        while (std::getline(token_stream, param,
            COL_CREATE_DELIM)) {  // each group divide by space
            params.push_back(param);
        }

        // invalid column create syntax
        if ((params.size() > 3 || params.size() < 2) ||
            (params.size() == 3 && is_key_found)) {
            switch (params.size()) {
            case 3:
                std::cout << "key has been specified more than once \n";
                break;
            default:
                std::cout << "invalid amount of specifiers should be 2 for "
                    "regular column or 3 for PK column \n";
                break;
            }
            throw(1);
        }

        // the name of the column should be second after the type name
        std::string col_name = params[1];
        std::variant<std::vector<int>, std::vector<double>, std::vector<char>,
            std::vector<std::string>>
            entries;
        if (params[0] == "INT") {
            entries = std::vector<int>();
        }
        else if (params[0] == "DOUBLE") {
            entries = std::vector<double>();
        }
        else if (params[0] == "CHAR") {
            entries = std::vector<char>();
        }
        else if (params[0] == "STR") {
            entries = std::vector<std::string>();
        }
        else {
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

            // set the boolean value of is_primary_key of column to true to
            // indicate that this is the PK
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

std::vector<column> table::get_contents() const { return contents; }

bool table::make_result_column(column& column_to_add,
    const column& compare_column_itr,
    const entry rvalue, std::string op) {
    bool is_empty_flag = true;
    std::visit(
        [rvalue, op, &column_to_add, &is_empty_flag]<class T>(
            const std::vector<T> &compare_column_entries) {
        for (const T& current_entry : compare_column_entries) {
            // if entry matches the op rvalue then add the entry to provided
            // column
            if (interpreter::compare_values(current_entry, rvalue, op)) {
                column_to_add.add_entry(current_entry);
                is_empty_flag = false;
            }
        }
    },
        compare_column_itr.entries);

    return is_empty_flag;
}

void table::add_matching_rows(
    table& table_to_mod,
    const std::vector<column>::const_iterator& compare_column,
    const entry rvalue, std::string op,
    std::vector<size_t>& captured_row_indicies) const {
    std::visit(
        [&table_to_mod, rvalue, op, &captured_row_indicies,
        this]<class T>(const std::vector<T> &compare_column) {
        for (size_t i = 0; i < compare_column.size(); ++i) {
            // check if current row already has been captured
            if (std::find(captured_row_indicies.begin(),
                captured_row_indicies.end(),
                i) != captured_row_indicies.end()) {
                continue;
            }
            // add the whole at index i if entry matches
            if (interpreter::compare_values(compare_column[i], rvalue,
                op)) {
                captured_row_indicies.push_back(i);
                table_to_mod.add_row_from_table(*this, i);
            }
        }
    },
        compare_column->entries);
}
table* table::read_table(const std::string& statement) const {
    // pointer to a new table to be returned at the end after feeling it with
    // filtered content according to statement
    table* ptr_result_table = new table();
    ptr_result_table->copy_empty_columns(contents);

    std::vector<std::string> tokens =
        interpreter::tokenizer(statement, table::READ_DELIM);
    std::vector<size_t> captured_row_indicies;

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::stringstream token_stream(tokens[i]);

        std::string col_name;
        std::string op;
        std::string rvalue;

        // spliting into 3 components (lvalue) (< or < or == or >= or <=)
        // (rvalue)
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

        // creating a variant to compare the rvalue to the values inside the
        // column
        entry rvalue_converted;
        column col_to_add;
        col_to_add.name = current_col->name;

        // using the right conversion based on type extracted'
        if (std::holds_alternative<std::vector<int>>(current_col->entries)) {
            rvalue_converted = std::stoi(rvalue);
            col_to_add.entries = std::vector<int>();
        }
        else if (std::holds_alternative<std::vector<double>>(
            current_col->entries)) {
            rvalue_converted = std::stod(rvalue);
            col_to_add.entries = std::vector<double>();
        }
        else if (std::holds_alternative<std::vector<char>>(
            current_col->entries)) {
            rvalue_converted = rvalue[0];
            col_to_add.entries = std::vector<char>();
        }
        else if (std::holds_alternative<std::vector<std::string>>(
            current_col->entries)) {
            rvalue_converted = rvalue;
            col_to_add.entries = std::vector<std::string>();
        }
        else {
            std::cout << "column \'" << current_col->name
                << "\' holds invalid type \n";
            throw(1);
        }

        add_matching_rows(*ptr_result_table, current_col, rvalue_converted, op,
            captured_row_indicies);
    }
    return ptr_result_table;
}

int table::change_primary_key(column* primary_key) {
    if (primary_key == nullptr) return 1;
    this->primary_key = primary_key;
    return 0;
}

void table::print_table() const {
    for (const column& col : contents) {
        std::cout << col.name << ": ";
        col.print_column();
    }
}

void column::print_column() const {
    std::visit(
        []<class T>(const std::vector<T> &entries_to_print) {
        if (entries_to_print.size() > 0) {
            std::copy(entries_to_print.begin(), entries_to_print.end() - 1,
                std::ostream_iterator<T>(std::cout, ", "));
            std::cout << entries_to_print.back() << std::endl;
        }
    },
        entries);
}

void table::add_row_from_table(const table& original_table, size_t row_index) {
    for (size_t i = 0; i < original_table.get_contents().size(); ++i) {
        contents[i].add_entry(
            original_table.get_contents()[i].get_entry(row_index));
    }
}

void table::copy_empty_columns(const std::vector<column>& columns) {
    for (const column& col : columns) {
        if (std::holds_alternative<std::vector<int>>(col.entries)) {
            contents.emplace_back(col.name, std::vector<int>());
        }
        else if (std::holds_alternative<std::vector<double>>(col.entries)) {
            contents.emplace_back(col.name, std::vector<double>());
        }
        else if (std::holds_alternative<std::vector<char>>(col.entries)) {
            contents.emplace_back(col.name, std::vector<char>());
        }
        else if (std::holds_alternative<std::vector<std::string>>(
            col.entries)) {
            contents.emplace_back(col.name, std::vector<std::string>());
        }
    }
}

column* const table::get_primary_key() const { return primary_key; }

void table::update_table(const std::string& statement) {
    std::vector<std::string> tokens =
        interpreter::tokenizer(statement, UPDATE_DELIM);
    std::vector<entry> row;
    if (tokens.size() != contents.size()) {
        std::cout << "amount of entries does not match the amount of columns "
            "in the table\n";
        throw(1);
    }
    // use the correct conversion based on the object held by column at index i
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (std::holds_alternative<std::vector<int>>(contents[i].entries)) {
            row.push_back(std::stoi(tokens[i]));
        }
        else if (std::holds_alternative<std::vector<double>>(
            contents[i].entries)) {
            row.push_back(std::stod(tokens[i]));
        }
        else if (std::holds_alternative<std::vector<char>>(
            contents[i].entries)) {
            row.push_back(tokens[i][0]);
        }
        else if (std::holds_alternative<std::vector<std::string>>(
            contents[i].entries)) {
            row.push_back(tokens[i]);
        }
        else {
            std::cout << "type is not supported\n";
            throw(1);
        }
    }

    add_new_row(row);
}
void table::add_new_row(std::vector<entry> row) {
    size_t i = 0;
    for (; i < row.size(); ++i) {
        contents[i].entries = std::visit(
            [row, i]<class T>(std::vector<T> &current_col_entries) -> var_vec {
            current_col_entries.push_back(std::get<T>(row[i]));

            return current_col_entries;
        },
            contents[i].entries);
    }
}

void table::drop_row(size_t index) {
    std::for_each(contents.begin(), contents.end(), [index](column& col) {
        col.entries = std::visit(
            [index]<class T>(std::vector<T> entries) -> var_vec {
            entries.erase(entries.begin() + index);
            return entries;
        },
            col.entries);
        });
}

size_t table::get_table_id() const { return id; }
std::string table::get_table_name() const { return table_name; }

void table::log_current_state() {
    char time_str[26];

    auto givetime =

        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ctime_s(time_str, sizeof(time_str), &givetime);
    state_logger.log_message(std::string("date: ") + time_str);
    std::for_each(
        contents.begin(), contents.end(), [this](const column& current_col) {
            state_logger.log_message(current_col.name, '\n');
            std::string entries_to_log;
            std::visit(
                [&entries_to_log]<class T>(const std::vector<T> &entries) {
                for (const T current_entry : entries) {
                    if constexpr (std::is_same<T, int>::value ||
                        std::is_same<T, double>::value) {
                        entries_to_log += std::to_string(current_entry);
                    }
                    else {
                        entries_to_log += current_entry;
                    }
                    entries_to_log += " ";
                }
            },
                current_col.entries);
            state_logger.log_message(entries_to_log, '\n');
        });
    state_logger.log_message("\n");
}

size_t table::get_id() const { return id; }

void column::serialize(const column& col, std::ofstream& out) {
    size_t size_of_name = col.name.size();
    out.write(reinterpret_cast<const char*>(&size_of_name),
        sizeof(size_of_name));
    out.write(col.name.c_str(), size_of_name);

    size_t entry_type = col.entries.index();
    out.write(reinterpret_cast<char*>(&entry_type), sizeof(entry_type));

    std::visit(
        [&out]<class T>(const std::vector<T> &vec_type) {
        size_t entry_amount = vec_type.size();
        out.write(reinterpret_cast<char*>(&entry_amount),
            sizeof(entry_amount));
    },
        col.entries);

    std::visit(
        [&out]<class T>(const std::vector<T> &vec_type) {
        for (T current_entry : vec_type) {
            if constexpr (std::is_same<T, std::string>::value) {
                std::string str_entry = current_entry;
                size_t str_length = str_entry.size();

                out.write(reinterpret_cast<char*>(&str_length),
                    sizeof(str_length));
                out.write(str_entry.c_str(), str_length);
            }
            else {
                out.write(reinterpret_cast<char*>(&current_entry),
                    sizeof(current_entry));
            }
        }
    },
        col.entries);
}

void column::deserialize(column& col, std::ifstream& in) {
    size_t size_of_name = 0;
    in.read(reinterpret_cast<char*>(&size_of_name), sizeof(size_of_name));
    col.name.resize(size_of_name);
    in.read(&col.name[0], size_of_name);

    size_t var_index = 0;
    in.read(reinterpret_cast<char*>(&var_index), sizeof(var_index));

    size_t amount_of_entries = 0;
    in.read(reinterpret_cast<char*>(&amount_of_entries),
        sizeof(amount_of_entries));

    switch (var_index) {
    case 0:
        col.entries = std::vector<int>();
        break;
    case 1:
        col.entries = std::vector<double>();
        break;
    case 2:
        col.entries = std::vector<char>();
        break;
    case 3:
        col.entries = std::vector<std::string>();
        break;
    default:
        std::cout << "unknown var_index index: " << var_index;
        break;
    }
    std::visit(
        [&in, amount_of_entries]<class T>(std::vector<T> &entries) {
        for (size_t i = 0; i < amount_of_entries; ++i) {
            if constexpr (std::is_same<T, std::string>::value) {
                size_t string_len = 0;
                in.read(reinterpret_cast<char*>(&string_len),
                    sizeof(string_len));
                std::string entry_str;
                entry_str.resize(string_len);
                in.read(&entry_str[0], string_len);
                entries.push_back(entry_str);
            }
            else {
                T entry_to_add;
                in.read(reinterpret_cast<char*>(&entry_to_add),
                    sizeof(entry_to_add));
                entries.push_back(entry_to_add);
            }
        }
    },
        col.entries);
}

void table::serialize(table& serialize_table, std::ofstream& out) {

    out.write(reinterpret_cast<char*>(&serialize_table.id),
        sizeof(serialize_table.id));

    size_t name_length = serialize_table.table_name.size();
    out.write(reinterpret_cast<char*>(&name_length), sizeof(name_length));

    out.write(serialize_table.table_name.c_str(), name_length);

    column::serialize(*serialize_table.primary_key, out);

    size_t contents_size = serialize_table.contents.size();
    out.write(reinterpret_cast<char*>(&contents_size), sizeof(contents_size));

    for (column col : serialize_table.contents) {
        column::serialize(col, out);
    }

    logger::serialize(serialize_table.state_logger, out);
   
}
table* table::deserialize(std::string name) {
    std::ifstream in(file_paths::TABLE_DAT_FILE_LOCATION + name + ".dat",
        std::ios::binary);
    size_t id;
    in.read(reinterpret_cast<char*>(&id), sizeof(id));
    size_t name_length = 0;
    in.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));
    std::string table_name;
    table_name.resize(name_length);
    in.read(&table_name[0], name_length);
    column primary_key;
    column::deserialize(primary_key, in);
    size_t contents_size = 0;
    in.read(reinterpret_cast<char*>(&contents_size), sizeof(contents_size));
    std::vector<column> contents;

    for (size_t i = 0; i < contents_size; ++i) {
        column col_to_add;
        column::deserialize(col_to_add, in);
        contents.push_back(col_to_add);
    }
    logger state_logger;
    logger::deserialize(state_logger, in);
    in.close();

    column* p_primary_key = new column(primary_key.name, primary_key.entries);
    table* p_table = new table(contents, p_primary_key, id, table_name);
    p_table->state_logger = state_logger;
    return p_table;
}