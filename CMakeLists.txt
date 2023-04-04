cmake_minimum_required(VERSION 3.17)
project(knife_lang)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake
        )

add_subdirectory(sqlite3)
add_subdirectory(antlr4-runtime)
add_subdirectory(compiler)
add_subdirectory(basekit)
add_subdirectory(user_prog)
add_subdirectory(tests)
