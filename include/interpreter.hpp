#pragma once

#include <stack>

#include "common.hpp"
namespace interpreter {
enum operators { AND, OR, LEFT_PAR, RIGHT_PAR };
const std::unordered_map<std::string, operators> operator_map = {
    {"&", operators::AND},
    {"||", operators::OR},
    {"(", operators::LEFT_PAR},
    {")", operators::RIGHT_PAR}};
const std::unordered_map<std::string, int> operation_precedence = {{"&", 1},
                                                                   {"||", 1}};
// simple tokenizer that return a vector of strings which were seperated by
// delim in statement
std::vector<std::string> tokenizer(const std::string &statement, char delim);

// comparing values of two std::variant base on given comparison operator
bool compare_values(const entry &lvalue, const entry &rvalue,
                    std::string comp_operator);

// statements have to be like this: COMMAND condition -> COMMAND condition ->
// ... example: FROM table_name -> SELECT col_name1,col_name2 -> WHERE col_name3
// > 30,col_name4 == D returns a map with keys as the desired command and values
// as the conditions
std::unordered_map<std::string, std::string> parse_statement(
    const std::string &statement);

void infix_to_postfix(const std::vector<std::string> &infix,
                      std::vector<std::string> &postfix);

}  // namespace interpreter