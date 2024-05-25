#include "../include/common.hpp"
#include "../include/dbms.hpp"
#include <gtest/gtest.h>

dbms create_dbms() {
    std::vector<table*> tables;
    std::string create_statment = "INT int_col PK,DOUBLE dob_col,CHAR char_col,STR str_col";
    for (size_t i = 0; i < 1000; ++i) {
        tables.emplace_back(new table(create_statment));
        tables.front()->update_table("6,1.3,B,hello world");
    }

    return dbms(tables);
}

TEST(get_table_test, get_table) {
    dbms test_dbms = create_dbms();
    table* test_table = new table("INT int_col PK,DOUBLE dob_col,CHAR char_col,STR str_col");
    test_table->update_table("6,1.3,B,hello world");
    GTEST_ASSERT_EQ(test_dbms.get_table("a name")->get_contents().at(0).get_entry(0), test_table->get_contents().at(0).get_entry(0));
}

TEST(add_table_test, add_table) {

}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
