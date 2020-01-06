cmake_minimum_required(VERSION 2.8.4)
project(quixo-cpp)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(quixo-cpp main.cpp)
add_executable(quixo-cpp-test test.cpp)