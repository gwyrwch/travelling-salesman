cmake_minimum_required(VERSION 3.14)
project(config)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_LIB DatasetConfig.cpp CacheConfig.cpp)

add_library(config STATIC ${SOURCE_LIB})
