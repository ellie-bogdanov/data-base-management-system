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

column::column(const column& copy) {
    name = copy.name;
    entries = copy.entries;
}

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

table::table(const std::vector<column>& contents, 
    size_t id, const std::string& table_name) : contents(contents), 
    id(id), table_name(table_name){

}

table::table(std::ifstream& in) {
    deserialize(in);
}

table::table() {
    id = 0;
    table_name = "NULL";
}

table::table(const std::vector<column::col_name_type>& col_types_names, size_t pk_index,
    const std::string& table_name, size_t id)
    : table_name(table_name), id(id) {
    column pk_col;
    for (size_t i = 0; i < col_types_names.size(); ++i) {

        column col;

        switch (col_types_names[i].second) {
        case entry_type::INT:
            col.name = col_types_names[i].first;
            col.entries = std::vector<int>();
            break;
        case entry_type::DOUBLE:
            col.name = col_types_names[i].first;
            col.entries = std::vector<double>();
            break;
        case entry_type::CHAR:
            col.name = col_types_names[i].first;
            col.entries = std::vector<char>();
            break;
        case entry_type::STRING:
            col.name = col_types_names[i].first;
            col.entries = std::vector<std::string>();
            break;
        }
        if (i == pk_index) {
            pk_col.name = col.name;
            pk_col.entries = col.entries;
            continue;
        }
        contents.push_back(col);
    }
    contents.insert(contents.begin(), pk_col);
}




table::table(const table& copy) {
    id = copy.id;
    table_name = copy.table_name;
    contents = copy.contents;
}


void table::set_id(size_t id) { this->id = id; }

table::~table() {

}

void table::change_pk(size_t new_pk_index) {
    column new_pk(contents[new_pk_index]);
    contents.erase(std::find(contents.begin(), contents.end(), contents[new_pk_index]));
    contents.insert(contents.begin(), new_pk);
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

table* table::read_table(std::vector<interpreter::column_comparison>& comparisons) {
    
    table *result = new table();
    
    std::unordered_map<std::string, size_t> name_index_map;
    for (interpreter::column_comparison col_comp : comparisons) {
        if (name_index_map.count(col_comp.col_name) == 0) {
            bool flag = false;
            for (size_t i = 0; i < contents.size(); ++i) {
                if (contents[i].name == col_comp.col_name) {
                    name_index_map[contents[i].name] = i;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                std::cout << "invalid column name: " << col_comp.col_name << '\n';
                exit(1);
            }
            
        }
        std::set<size_t> row_indicies;
        
        std::visit([&result, col_comp, &row_indicies]<class T>(std::vector<T> col_entries) {
            
            for (size_t i = 0; i < col_entries.size(); ++i) {
                if constexpr (std::is_same<std::string, T>::value) {
                    if (col_comp.op != interpreter::compare_operators::EQ) {
                        std::cout << "can only compare string with == \n";
                        exit(1);
                    }
                    if (col_entries[i] == std::get<std::string>(col_comp.compare_to)) {
                        row_indicies.insert(i);
                    }
                }
                else {
                    switch (col_comp.op) {
                    case interpreter::compare_operators::MT:
                        if (col_entries[i] > std::get<T>(col_comp.compare_to)) {
                            row_indicies.insert(i);
                        }
                        break;
                    case interpreter::compare_operators::LT:
                        if (col_entries[i] < std::get<T>(col_comp.compare_to)) {
                            row_indicies.insert(i);

                        }
                        break;
                    case interpreter::compare_operators::MOE:
                        if (col_entries[i] >= std::get<T>(col_comp.compare_to)) {
                            row_indicies.insert(i);
                        }
                        break;
                    case interpreter::compare_operators::LOE:
                        if (col_entries[i] <= std::get<T>(col_comp.compare_to)) {
                            row_indicies.insert(i);
                        }
                        break;
                    default:
                        if (col_entries[i] == std::get<T>(col_comp.compare_to)) {
                            row_indicies.insert(i);
                        }
                        break;
                    }
                }
                
            }

        }, contents[name_index_map[col_comp.col_name]].entries);
       
        
        size_t content_size = contents.size();
        for (auto it = row_indicies.begin(); it != row_indicies.end(); ++it) {
            std::vector<entry> row;
            for (size_t i = 0; i < content_size; ++i) {
                std::visit([&row, i, content_size, it]<class T>(std::vector<T> column_entries) {
                    row.push_back(column_entries[*it]);
                }, contents[content_size].entries);
            }
            result->add_new_row(row);
        }
        
    }
    
    return result;
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

size_t table::get_table_size() {
    size_t table_size_bytes = sizeof(id) + table_name.size();

    for (size_t i = 0; i < contents.size(); ++i) {
        std::visit([&table_size_bytes]<class T>(const std::vector<T>& col_contents) {
            if constexpr (std::is_same<std::string, T>::value) {
                for (size_t j = 0; j < col_contents.size(); ++j) {
                    table_size_bytes += col_contents.at(j).size();
                }
            }
            else {
                table_size_bytes += col_contents.size() + sizeof(T);
            }
                
        },
            contents[i].entries);
    }
    return table_size_bytes;
}

void column::serialize(std::ofstream& out) const {

    serializer::serialize(out, this->name);
    std::visit([&out]<class T>(const std::vector<T> &vec) {
        entry_type type;
        if (std::is_same<T, int>::value) {
            type = entry_type::INT;
        }
        else if (std::is_same<T, double>::value) {
            type = entry_type::DOUBLE;
        }
        else if (std::is_same<T, char>::value) {
            type = entry_type::CHAR;
        }
        else if (std::is_same<T, std::string>::value) {
            type = entry_type::STRING;
        }
        serializer::serialize(out, type, vec);
    }, entries);
}
void column::deserialize(std::ifstream& in) {
    entry_type type;
    var_vec entries;

    serializer::deserialize(in, this->name, type);
    switch (type) {
    case entry_type::INT:
        entries = std::vector<int>();
        break;

    case entry_type::DOUBLE:
        entries = std::vector<double>();

        break;

    case entry_type::CHAR:
        entries = std::vector<char>();

        break;

    case entry_type::STRING:
        entries = std::vector<std::string>();
        break;
    default:
        break;

    }

    this->entries = std::visit([&in]<class T>(std::vector<T> &vec) -> var_vec {
        serializer::deserialize(in, vec);
        return vec;
    }, entries);

}

void table::serialize(std::ofstream& out) const {
    size_t contents_size = this->contents.size();
    serializer::serialize(out, this->id, this->table_name, contents_size);
    for (size_t i = 0; i < this->contents.size(); ++i) {
        this->contents[i].serialize(out);
    }
}

void table::deserialize(std::ifstream& in) {
    size_t contents_size = 0;
    serializer::deserialize(in, id, table_name, contents_size);
    for (size_t i = 0; i < contents_size; ++i) {
        column col;
        col.deserialize(in);
        contents.push_back(col);
    }
}