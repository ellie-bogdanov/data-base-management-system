#pragma once

#include "logger.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using std::size_t;
using var_vec = std::variant<std::vector<int>, std::vector<double>, std::vector<char>, std::vector<std::string>>;
using entry = std::variant<int, double, char, std::string>;
