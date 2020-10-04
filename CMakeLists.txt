cmake_minimum_required(VERSION 3.17)
project(cppsenioreas_iterators)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread -Wall -Wextra -pedantic")

add_executable(cppsenioreas_iterators main.cpp)