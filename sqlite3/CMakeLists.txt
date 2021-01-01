cmake_minimum_required(VERSION 3.17)
project(sqlite3)

add_compile_definitions(SQLITE_ENABLE_JSON1)

add_library(sqlite_lib
        sqlite3.h
        sqlite3.c
        )

target_include_directories(sqlite_lib PUBLIC .)

add_executable(sqlite3
        shell.c)

target_link_libraries(sqlite3 sqlite_lib)
