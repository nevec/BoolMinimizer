cmake_minimum_required(VERSION 3.9)
project(Kursach)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-D_GLIBCXX_DEBUG -fsanitize=address")
add_executable(Kursach main.cpp tokens.h)