#pragma once
#include "common.hpp"
#include <sstream>
#include <type_traits>

namespace serializer {

	static void serialize(std::ofstream& out) {

		return;
	}
	static void deserialize(std::ifstream& in) {
		return;
	}

	template<typename T, typename... Ts>
	static void serialize(std::ofstream& out, std::vector<T> &vec_arg, Ts... args) {
		size_t arg_length = vec_arg.size();
		size_t arg_size = sizeof(T);
		out.write(reinterpret_cast<char*>(&arg_length), sizeof(arg_length));
		if constexpr (std::is_same<T, std::string>::value) {
			for (std::string str : vec_arg) {
				size_t str_len = str.size();
				out.write(reinterpret_cast<char*>(&str_len), sizeof(str_len));
				out.write(str.data(), str_len);
			}
		}
		else {
			out.write(reinterpret_cast<char*>(&arg_size), sizeof(arg_size));
			out.write(reinterpret_cast<char*>(vec_arg.data()), arg_length * arg_size);
		}

		serialize(out, args...);
	}


	template<typename T, typename... Ts>
	static void serialize(std::ofstream &out, T arg, Ts... args) {
		if constexpr (std::is_same<T, std::string>::value) {
			/*std::stringstream arg_sstream;
			arg_sstream << arg;
			std::string str_to_ser = arg_sstream.str();
			*/
			size_t str_len = arg.size();
			out.write(reinterpret_cast<char*>(&str_len), sizeof(str_len));
			out.write(arg.data(), str_len);
		}
		
		else {
			out.write(reinterpret_cast<char*>(&arg), sizeof(arg));
		}

		serialize(out, args...);
	}

	
	template<typename T, typename... Ts>
	static void deserialize(std::ifstream& in, std::vector<T>& vec_arg, Ts... args) {
		size_t arg_length = 0;
		size_t arg_size = 0;
		in.read(reinterpret_cast<char*>(&arg_length), sizeof(arg_length));
	
		vec_arg.resize(arg_length);
		if constexpr (std::is_same<T, std::string>::value) {
			for (size_t i = 0; i < arg_length; ++i) {
				size_t str_len = 0;
				in.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));
				std::string str;
				str.resize(str_len);
				in.read(str.data(), str_len);
				vec_arg.push_back(str);
			}
		}
		else {
			in.read(reinterpret_cast<char*>(&arg_size), sizeof(arg_size));
			in.read(reinterpret_cast<char*>(vec_arg.data()), arg_length * arg_size);
		}

		deserialize(in, args...);
	}

	template<typename T, typename... Ts>
	static void deserialize(std::ifstream& in, T &arg, Ts&... args) {
		if constexpr (std::is_same<T, std::string>::value) {
			size_t str_len = 0;
			in.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));

			arg.resize(str_len);
			in.read(arg.data(), str_len);
		}
		else {
			in.read(reinterpret_cast<char*>(&arg), sizeof(arg));
		}
		deserialize(in, args...);
	}
}