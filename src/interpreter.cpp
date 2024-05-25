#include "../include/interpreter.hpp"

#include <sstream>

std::vector<std::string> interpreter::tokenizer(const std::string &statement,
                                                char delim) {
    std::vector<std::string> tokens;
    std::stringstream token_stream(statement);
    std::string current_token;
    while (std::getline(token_stream, current_token, delim)) {
        tokens.push_back(current_token);
    }

    return tokens;
}

// checking all options of comparisons
bool interpreter::compare_values(const entry &lvalue, const entry &rvalue,
                                 std::string comp_operator) {
    bool is_valid_comp_op = true;
    bool compare_result = (comp_operator == "==")   ? (lvalue == rvalue)
                          : (comp_operator == ">")  ? (lvalue > rvalue)
                          : (comp_operator == "<")  ? (lvalue < rvalue)
                          : (comp_operator == "<=") ? (lvalue <= rvalue)
                          : (comp_operator == ">=") ? (lvalue >= rvalue)
                                                    : is_valid_comp_op = false;

    if (!is_valid_comp_op) {
        std::cout << "invalid comparison operator '" << comp_operator << "' \n";
        throw(1);
    }

    return compare_result;
}

void interpreter::infix_to_postfix(const std::vector<std::string> &infix,
                                   std::vector<std::string> &postfix) {
    std::stack<std::string> intermidiate;
    using namespace interpreter;
    auto single_operation = [&postfix,
                             &intermidiate](const std::string &value) {
        if (operator_map.find(value) != operator_map.end()) {
            if (value != "(" && value != ")") {
                while (!intermidiate.empty() && intermidiate.top() != "(" &&
                       operation_precedence.at(value) <=
                           operation_precedence.at(intermidiate.top())) {
                    postfix.push_back(intermidiate.top());
                    intermidiate.pop();
                }
            }
            switch (operator_map.at(value)) {
                case operators::AND:
                    intermidiate.push(value);
                    break;
                case operators::OR:
                    intermidiate.push(value);
                    break;
                case operators::LEFT_PAR:
                    intermidiate.push(value);
                    break;
                case operators::RIGHT_PAR:
                    while (intermidiate.top() != "(") {
                        postfix.push_back(intermidiate.top());
                        intermidiate.pop();
                    }
                    intermidiate.pop();
                    break;
                default:
                    std::cout << "operator '" << value << "' not supported\n";
                    throw(1);
                    break;
            }

        } else {
            postfix.push_back(value);
        }
    };

    std::for_each(infix.cbegin(), infix.cend(), single_operation);
    while (!intermidiate.empty()) {
        postfix.push_back(intermidiate.top());
        intermidiate.pop();
    }
}

std::unordered_map<std::string, std::string> interpreter::parse_statement(
    const std::string &statement) {
    // TODO: finish
}