#include "../include/table.hpp"
#include <gtest/gtest.h>
#include <random>

int rnd_int(int start_range, int end_range) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<int> dist(start_range, end_range);

    return dist(rng);
}
int rnd_double(int start_range, int end_range) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<double> dist(start_range, end_range);

    return dist(rng);
}

entry get_entry_test(size_t index, column &col) {
    return col.get_entry(index);
}

TEST(entry_test, get_entry_test) {
    std::vector<int> int_vec;
    std::vector<double> double_vec;
    std::vector<char> char_vec;
    std::vector<std::string> str_vec;

    for (size_t i = 0; i < 1000; ++i) {
        int_vec.push_back(rnd_int(-1000, 1000));
        double_vec.push_back(rnd_double(-1000.0, 1000.0));
        char_vec.push_back(rnd_int(0, 255));

        std::string str = "";
        size_t str_len = rnd_int(1, 10);
        for (size_t j = 0; j < str_len; ++j) {
            str.push_back(rnd_int(0, 255));
        }
        str_vec.push_back(str);
    }

    column int_col("int_col", int_vec);
    column double_col("double_col", double_vec);
    column char_col("char_col", char_vec);
    column str_col("str_col", str_vec);

    size_t rand_index = rnd_int(0, 999);

    GTEST_ASSERT_EQ(std::get<int>(get_entry_test(rand_index, int_col)), int_vec[rand_index]);
    GTEST_ASSERT_EQ(std::get<double>(get_entry_test(rand_index, double_col)), double_vec[rand_index]);
    GTEST_ASSERT_EQ(std::get<char>(get_entry_test(rand_index, char_col)), char_vec[rand_index]);
    GTEST_ASSERT_EQ(std::get<std::string>(get_entry_test(rand_index, str_col)), str_vec[rand_index]);
}
/*
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/