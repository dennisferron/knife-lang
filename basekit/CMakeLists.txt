cmake_minimum_required(VERSION 3.17)
project(basekit)

add_library(basekit STATIC
    Environment.cpp
    Environment.hpp
        DataTypes.cpp DataTypes.hpp)

target_include_directories(basekit PUBLIC .)
