cmake_minimum_required(VERSION 3.14)
project(util)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_LIB String.cpp)

add_library(util STATIC ${SOURCE_LIB})
#target_link_libraries()
