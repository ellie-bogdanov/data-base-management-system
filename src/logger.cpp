#include "../include/logger.hpp"

logger::logger(const std::string& file_name, std::string file_extension)
    : file_name(file_name) {
    full_path =
        logger_file_paths::LOG_FILE_LOCATION + file_name + "." + file_extension;

    if (file_extension.front() == '.') {
        file_extension.erase(0);
    }
    this->file_extension = file_extension;
}

logger::logger() {}

logger::logger(const std::string& file_name)
    : file_name(file_name), file_extension("txt") {
    full_path =
        logger_file_paths::LOG_FILE_LOCATION + file_name + "." + file_extension;
}

void logger::log_message(const std::string& message, char delim) {
    std::ofstream log_file_output(full_path, std::ofstream::app);
    log_file_output << message << delim;
    log_file_output.close();
}

void logger::log_message(const std::string& message) {
    std::ofstream log_file_output(full_path, std::ofstream::app);
    log_file_output << message;
    log_file_output.close();
}

std::string logger::retrieve_log_file_contents() {
    std::ifstream log_file_input(full_path);
    log_file_input.open(full_path, std::ifstream::in);
    std::cout << file_name << "\n";
    std::string full_file_contents = "";
    if (log_file_input.is_open()) {
        std::string current_line;
        while (std::getline(log_file_input, current_line, '\n')) {
            std::cout << current_line << '\n';
            full_file_contents.append(current_line + "\n");
        }
        log_file_input.close();
    } else {
        std::cout << "unable to open file " << file_name << "\n";
        throw(1);
    }
    return full_file_contents;
}

logger::~logger() {}

void logger::serialize(const logger& ser_logger, std::ofstream& out) {
    size_t full_path_size = 0;
    out.write(reinterpret_cast<char*>(&full_path_size), sizeof(full_path_size));
    out.write(ser_logger.full_path.c_str(), full_path_size);

    size_t file_name_size = 0;
    out.write(reinterpret_cast<char*>(&file_name_size), sizeof(file_name_size));
    out.write(ser_logger.file_name.c_str(), file_name_size);

    size_t file_extension_size = 0;
    out.write(reinterpret_cast<char*>(&file_extension_size),
              sizeof(file_extension_size));
    out.write(ser_logger.file_extension.c_str(), file_extension_size);
}
void logger::deserialize(logger& des_logger, std::ifstream& in) {
    size_t full_path_size = 0;
    in.read(reinterpret_cast<char*>(&full_path_size), sizeof(full_path_size));
    des_logger.full_path.resize(full_path_size);
    in.read(&des_logger.full_path[0], full_path_size);

    size_t file_name_size = 0;
    in.read(reinterpret_cast<char*>(&file_name_size), sizeof(file_name_size));
    des_logger.file_name.resize(file_name_size);
    in.read(&des_logger.file_name[0], file_name_size);

    size_t file_extension_size = 0;
    in.read(reinterpret_cast<char*>(&file_extension_size),
            sizeof(file_extension_size));
    des_logger.file_extension.resize(file_extension_size);
    in.read(&des_logger.file_extension[0], file_extension_size);
}