cmake_minimum_required(VERSION 3.9)
project(Kursach)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG)
add_executable(Kursach main.cpp parser.cpp tokens.h parser.h tokens.cpp minimizer.h minimizer.cpp)