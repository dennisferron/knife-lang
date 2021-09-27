cmake_minimum_required(VERSION 3.16)
project(knife-tests)

set(CMAKE_CXX_STANDARD 20)

#set( CMAKE_VERBOSE_MAKEFILE on )

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake
        )

include(UseBoost)

add_executable(knife-tests
        TestMain.cpp
        EnvironmentTests.cpp
        CompilationTests.cpp
        Utility.hpp)

set_target_properties(knife-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiler")
target_link_libraries(knife-tests PUBLIC basekit knife_lib)
