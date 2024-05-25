#pragma once

#include <iostream>
#include <fstream>
#include <string>

struct logger
{
    constexpr static std::string LOG_FILE_LOCATION = "..\\logs\\";
    std::ofstream log_file_output;
    std::ifstream log_file_input;
    std::string full_path;
    std::string file_name;
    std::string file_extension;
    logger(const std::string &file_name, std::string file_extension);
    logger(const std::string &file_name);
    ~logger();
    void log_message(const std::string &message);
    std::string retrieve_log_file_contents();
};