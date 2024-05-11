#pragma once

#include "common.hpp"

namespace interpreter {
    // simple tokenizer that return a vector of strings which were seperated by delim in statement
    std::vector<std::string> tokenizer(const std::string &statement, char delim);

    // comparing values of two std::variant base on given comparison operator
    bool compare_values(const entry &lvalue, const entry &rvalue, std::string comp_operator);

    // statements have to be like this: COMMAND condition -> COMMAND condition -> ... example: FROM table_name -> SELECT col_name1,col_name2 -> WHERE col_name3 > 30,col_name4 == D
    // returns a map with keys as the desired command and values as the conditions
    std::unordered_map<std::string, std::string> parse_statement(const std::string &statement);

}