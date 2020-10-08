cmake_minimum_required(VERSION 3.17)
project(cppsenioreas_iterators)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread -Wall -Wextra -pedantic")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/home/sherlock/softwares/gcc/gcc-my-test/build/install/lib64/")

add_executable(cppsenioreas_iterators main.cpp)