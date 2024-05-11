
#include "../include/interpreter.hpp"
#include <gtest/gtest.h>

TEST(interpreter_tests, infix_to_postfix) {

    std::vector<std::string> infix = {"a", "&", "(", "b", "||", "c", ")", "&", "d"};
    std::vector<std::string> postfix;
    std::vector<std::string> postfix_test = {"a", "b", "c", "||", "&", "d", "&"};

    interpreter::infix_to_postfix(infix, postfix);

    GTEST_ASSERT_EQ(postfix, postfix_test);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}