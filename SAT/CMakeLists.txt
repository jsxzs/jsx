cmake_minimum_required(VERSION 3.19)
project(SAT)

set(CMAKE_CXX_STANDARD 11)

add_executable(SAT cnf.h sudoku.h DPLL.h main.cpp)