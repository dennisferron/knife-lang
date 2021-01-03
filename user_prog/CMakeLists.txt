cmake_minimum_required(VERSION 3.17)
project(user_prog)

set(CMAKE_CXX_STANDARD 20)

add_executable(user_prog
        user_main.cpp
        prog.cpp
        prog.hpp)

target_include_directories(user_prog PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR})

set(transpile_output output.json)
add_custom_command(
        COMMAND transpiler -o ${transpile_output} -e ${CMAKE_CURRENT_SOURCE_DIR}/t.expr
        DEPENDS t.expr
        OUTPUT ${transpile_output}
        COMMENT "Running transpiler to generate JSON data for templates."
)

set(stst_templates_dir ${CMAKE_SOURCE_DIR}/templates)

set(stst_output_hpp prog.hpp)
add_custom_command(
        COMMAND stst
            -o  ${stst_output_hpp}
            -t  ${CMAKE_SOURCE_DIR}/templates
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
        OUTPUT ${stst_output_sql}
        #TARGET user_prog PRE_LINK
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
