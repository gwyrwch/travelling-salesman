cmake_minimum_required(VERSION 3.14)
project(tsp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/Users/gwyrwch/Projects/course-work-5")

add_executable(tsp main.cpp algo/ISolution.cpp algo/Test.cpp algo/Tour.cpp)
include_directories(cxxopts/include)
