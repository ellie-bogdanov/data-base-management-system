cmake_minimum_required(VERSION 3.28)

project(dbms)

set(CMAKE_CXX_STANDARD 23)

enable_testing()

file(GLOB_RECURSE SOURCE_FILES src/*)
#file(GLOB_RECURSE TEST_FILES tests/*)

add_executable(dbms ${SOURCE_FILES})

#target_link_libraries(dbms PRIVATE gtest)

#include(GoogleTest)

#gtest_discover_tests(dbms)
