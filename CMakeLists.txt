cmake_minimum_required(VERSION 3.14)
project(tsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/Users/gwyrwch/Projects/course-work-5")

include_directories(cxxopts/include)
include_directories(.)


add_executable(tsp main.cpp)

add_subdirectory(lib)
add_subdirectory(runner)
add_subdirectory(algo)
add_subdirectory(config)


target_link_libraries(tsp runner)

