#include "../include/interpreter.hpp"

#include <sstream>


std::vector<std::string> interpreter::tokenizer(const std::string &statement, char delim) {
    std::vector<std::string> tokens;
    std::stringstream token_stream(statement);
    std::string current_token;
    while (std::getline(token_stream, current_token, delim)) {
        tokens.push_back(current_token);
    }

    return tokens;
}

// checking all options of comparisons
bool interpreter::compare_values(const entry &lvalue, const entry &rvalue, std::string comp_operator) {
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