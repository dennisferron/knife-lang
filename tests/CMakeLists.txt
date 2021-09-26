cmake_minimum_required(VERSION 3.16)
project(tests)

set(CMAKE_CXX_STANDARD 20)

#set( CMAKE_VERBOSE_MAKEFILE on )

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake
        )

include(UseBoost)

add_executable(tests
        TestMain.cpp
        EnvironmentTests.cpp
        CompilationTests.cpp
        Utility.hpp)

target_link_libraries(tests PUBLIC basekit knife_lib)
