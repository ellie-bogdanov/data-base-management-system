cmake_minimum_required(VERSION 3.28)
project(dbms)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCE_FILES src/*)
add_executable(dbms ${SOURCE_FILES})
