cmake_minimum_required(VERSION 3.14)
project(tsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./../")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

set(Boost_INCLUDE_DIR /usr/local/Cellar/boost/1.71.0/include)
set(Boost_LIBRARY_DIR /usr/local/Cellar/boost/1.71.0/lib)
find_package(Boost COMPONENTS system filesystem python37 numpy37 REQUIRED)


find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(include ${GTKMM_INCLUDE_DIRS})

pkg_check_modules(PYGOBJECT pygobject-3.0)
#
link_directories(${PYGOBJECT_LIBRARY_DIRS})
include_directories(include ${PYGOBJECT_INCLUDE_DIRS})

# /usr/local/Cellar/boost/1.71.0

include_directories(cxxopts/include)
include_directories(.)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

#include_directories(/usr/local/Cellar/python/3.7.5/Frameworks/Python.framework/Versions/3.7/include/python3.7m)

add_executable(tsp main.cpp)

add_subdirectory(lib)
add_subdirectory(runner)
add_subdirectory(algo)
add_subdirectory(config)
add_subdirectory(util)

target_link_libraries(tsp runner ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${GTKMM_LIBRARIES}
        ${PYGOBJECT_LIBRARIES}
        )
