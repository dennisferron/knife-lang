cmake_minimum_required(VERSION 3.17)
project(user_prog)

set(CMAKE_CXX_STANDARD 20)

add_executable(user_prog
        user_main.cpp
        prog.cpp
        prog.hpp)

target_include_directories(user_prog PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR})

set(transpile_inputs examples/family_con.kf)
set(transpile_base_name transpile_output)
set(transpile_output_json ${transpile_base_name}.json)
set(transpile_output_stg ${transpile_base_name}.stg)
set(transpile_outputs ${transpile_output_json} ${transpile_output_stg})
add_custom_command(
        COMMAND transpiler -o ${transpile_base_name} -i ${CMAKE_CURRENT_SOURCE_DIR} ${transpile_inputs}
        DEPENDS transpiler ${transpile_inputs}
        OUTPUT ${transpile_outputs}
        COMMENT "Running transpiler to generate JSON data for templates."
)

# Originally CMake only worked with one output from a custom command.
# A work-around was to add a do-nothing custom command depending on
# one output and pretending to produce the second output file.
# See https://cmake.org/pipermail/cmake/2006-March/008707.html
# HOWEVER, this was supposedly fixed in CMake version 3.2.1.
# See https://cmake.org/Bug/view.php?id=15116
# So the following should not be necessary after all.
#ADD_CUSTOM_COMMAND(
#    OUTPUT ${
#    COMMAND ${CMAKE_COMMAND} -E echo "Dummy command to recognize additional output file."
#    DEPENDS output.json
#)

set(stst_templates_dir ${CMAKE_SOURCE_DIR}/templates)

set(stst_output_hpp prog.hpp)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_hpp}
            -t  ${CMAKE_CURRENT_BINARY_DIR}
                ${transpile_base_name}.header
                ${transpile_output_json}
        DEPENDS ${transpile_outputs}
        OUTPUT  ${stst_output_hpp}
        COMMENT "X: ${transpile_output_json} Running StringTemplate Standalone Tool (stst) to generate header (hpp)."
)

set(stst_output_cpp prog.cpp)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_cpp}
            -t  ${CMAKE_CURRENT_BINARY_DIR}
                ${transpile_base_name}.source
                ${transpile_output_json}
        DEPENDS ${transpile_outputs}
        OUTPUT  ${stst_output_cpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate source (cpp)."
)

set(stst_output_sql prog.sql)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_sql}
            -t  ${CMAKE_CURRENT_BINARY_DIR}
                ${transpile_base_name}.database
                ${transpile_output_json}
        DEPENDS ${transpile_outputs}
        OUTPUT ${stst_output_sql}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate database script (sql)."
)

set(output_db prog.db)
add_custom_command(
        COMMAND sqlite3 ${output_db} ".read ${stst_output_sql}"
        DEPENDS ${stst_output_sql}
        OUTPUT ${output_db}
        #TARGET user_prog POST_BUILD
        COMMENT "Running Sqlite3 to generate database file (db)."
)

add_custom_target(
        prog_db ALL
        COMMENT "Dummy target to cause db to be generated."
        DEPENDS ${output_db}
)
