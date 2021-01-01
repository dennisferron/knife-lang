cmake_minimum_required(VERSION 3.17)
project(user_prog)

set(CMAKE_CXX_STANDARD 20)

add_executable(user_prog
        user_main.cpp
        prog.cpp
        prog.hpp)

target_include_directories(user_prog PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR})

set(transpile_inputs ${CMAKE_SOURCE_DIR}/samples/t.expr)
set(transpile_output output.json)

add_custom_command(
        COMMAND transpiler -o ${transpile_output} -e ${transpile_inputs}
        DEPENDS ${transpile_inputs}
        OUTPUT ${transpile_output}
        COMMENT "Running transpiler to generate JSON data for templates."
)

set(stst_templates_dir ${CMAKE_SOURCE_DIR}/templates)

set(stst_output_hpp prog.hpp)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_hpp}
            -t  ${stst_templates_dir}
                ExprProg.header
                ${transpile_output}
        DEPENDS ${transpile_output} ../templates/ExprProg.stg
        OUTPUT  ${stst_output_hpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate header (hpp)."
)

set(stst_output_cpp prog.cpp)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_cpp}
            -t  ${stst_templates_dir}
                ExprProg.source
                ${transpile_output}
        DEPENDS ${transpile_output} ../templates/ExprProg.stg
        OUTPUT  ${stst_output_cpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate source (cpp)."
)

set(stst_output_sql prog.sql)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_sql}
            -t  ${stst_templates_dir}
                SqlScripts.database
                ${transpile_output}
        DEPENDS ${transpile_output} ../templates/SqlScripts.stg
        #OUTPUT ${stst_output_sql}
        TARGET user_prog PRE_LINK
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate database script (sql)."
)
