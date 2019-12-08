cmake_minimum_required(VERSION 3.14)
project(lib)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_LIB Timer.cpp ThreadPool.cpp)

add_library(lib STATIC ${SOURCE_LIB})
