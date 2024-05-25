#include "../include/logger.hpp"

logger::logger(const std::string &file_name, std::string file_extension) : file_name(file_name)
{
    full_path = logger::LOG_FILE_LOCATION + file_name + "." + file_extension;
    if (file_extension.front() == '.')
    {
        file_extension.erase(0);
    }
    this->file_extension = file_extension;
    log_file_output = std::ofstream(full_path);
    log_file_output.close();
}

logger::logger(const std::string &file_name) : file_name(file_name), file_extension("txt")
{
    full_path = logger::LOG_FILE_LOCATION + file_name + "." + file_extension;
    log_file_output = std::ofstream(full_path);
    log_file_output.close();
}

void logger::log_message(const std::string &message)
{
    log_file_output.open(full_path, std::ofstream::app);
    log_file_output << message << '\n';
    log_file_output.close();
}

std::string logger::retrieve_log_file_contents()
{

    log_file_input.open(full_path, std::ifstream::in);
    std::cout << file_name << "\n";
    std::string full_file_contents = "";
    if (log_file_input.is_open())
    {
        std::string current_line;
        while (std::getline(log_file_input, current_line, '\n'))
        {
            std::cout << current_line << '\n';
            full_file_contents.append(current_line + "\n");
        }
        log_file_input.close();
    }
    else
    {
        std::cout << "unable to open file " << file_name << "\n";
        throw(1);
    }
    return full_file_contents;
}

logger::~logger()
{
    if (log_file_input.is_open())
        log_file_input.close();
    if (log_file_output.is_open())
        log_file_output.close();
}