cmake_minimum_required(VERSION 3.14)
project(runner)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_LIB SolutionsRunner.cpp)

add_library(runner STATIC ${SOURCE_LIB})
target_link_libraries(runner algo config lib)
