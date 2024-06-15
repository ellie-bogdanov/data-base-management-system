#include "../include/serializer.hpp"

void serializer::serialize_column(std::ofstream &out, const column &col) {
    size_t size_of_name = col.name.size();
    out.write(reinterpret_cast<const char *>(&size_of_name),
              sizeof(size_of_name));
    out.write(col.name.c_str(), size_of_name);

    size_t entry_type = col.entries.index();
    out.write(reinterpret_cast<char *>(&entry_type), sizeof(entry_type));

    std::visit(
        [&out]<class T>(const std::vector<T> &vec_type) {
            size_t entry_amount = vec_type.size();
            out.write(reinterpret_cast<char *>(&entry_amount),
                      sizeof(entry_amount));
        },
        col.entries);

    std::visit(
        [&out]<class T>(const std::vector<T> &vec_type) {
            for (T current_entry : vec_type) {
                if constexpr (std::is_same<T, std::string>::value) {
                    std::string str_entry = current_entry;
                    size_t str_length = str_entry.size();

                    out.write(reinterpret_cast<char *>(&str_length),
                              sizeof(str_length));
                    out.write(str_entry.c_str(), str_length);
                } else {
                    out.write(reinterpret_cast<char *>(&current_entry),
                              sizeof(current_entry));
                }
            }
        },
        col.entries);
}

void serializer::deserialize_column(std::ifstream &in, column &col) {
    size_t size_of_name = 0;
    in.read(reinterpret_cast<char *>(&size_of_name), sizeof(size_of_name));
    col.name.resize(size_of_name);
    in.read(&col.name[0], size_of_name);

    size_t var_index = 0;
    in.read(reinterpret_cast<char *>(&var_index), sizeof(var_index));

    size_t amount_of_entries = 0;
    in.read(reinterpret_cast<char *>(&amount_of_entries),
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
                    in.read(reinterpret_cast<char *>(&string_len),
                            sizeof(string_len));
                    std::string entry_str;
                    entry_str.resize(string_len);
                    in.read(&entry_str[0], string_len);
                    entries.push_back(entry_str);
                } else {
                    T entry_to_add;
                    in.read(reinterpret_cast<char *>(&entry_to_add),
                            sizeof(entry_to_add));
                    entries.push_back(entry_to_add);
                }
            }
        },
        col.entries);
}