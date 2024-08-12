#include "uid.hpp"

uid::uid() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto now_in_micros = std::chrono::duration_cast<std::chrono::microseconds>(now).count();

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<uint64_t> dis(0, 0xFFFFFFFFFFFF);

	uint64_t random_component = dis(gen);

	std::stringstream id_stream;
	id_stream << std::hex << std::setfill('0') << std::setw(16) << now_in_micros << std::setw(12) << random_component;

	id = id_stream.str();
}