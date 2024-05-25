#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "../file_paths/logger_file_paths.hpp"

struct logger {
    // const std::ofstream log_file_output;
    // const std::ifstream log_file_input;
    std::string full_path;
    std::string file_name;
    std::string file_extension;
    logger(const std::string &file_name, std::string file_extension);
    logger(const std::string &file_name);
    logger();
    ~logger();
    void log_message(const std::string &message, char delim);
    void log_message(const std::string &message);
    std::string retrieve_log_file_contents();
};