#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "file_paths.hpp"

struct logger {
    std::string full_path;
    std::string file_name;
    std::string file_extension;
    logger(const std::string& file_name, std::string file_extension);
    logger(const std::string& file_name);
    logger();
    ~logger();
    void log_message(const std::string& message, char delim);
    void log_message(const std::string& message);
    std::string retrieve_log_file_contents();

    static void serialize(const logger& ser_logger, std::ofstream& out);
    static void deserialize(logger& des_logger, std::ifstream& in);
};