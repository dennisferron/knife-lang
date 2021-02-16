cmake_minimum_required(VERSION 3.17)
project(user_prog)

set(CMAKE_CXX_STANDARD 20)

set(transpile_base_name user_prog)

set(json_header ${transpile_base_name}_header.json)
set(json_source ${transpile_base_name}_source.json)
set(json_db_init ${transpile_base_name}_db_init.json)

set(stst_output_hpp ${transpile_base_name}.hpp)
set(stst_output_cpp ${transpile_base_name}.cpp)
set(stst_output_sql ${transpile_base_name}.sql)

set(transpile_inputs examples/family_con.kf)
set(stst_templates_dir ${CMAKE_SOURCE_DIR}/templates)
set(sqlite_output_db ${transpile_base_name}.db)

add_executable(user_prog
        user_main.cpp
        user_prog.cpp
        user_prog.hpp)

target_include_directories(user_prog PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    COMMAND transpiler
        -o ${transpile_base_name}
        -i ${CMAKE_CURRENT_SOURCE_DIR} ${transpile_inputs}
    DEPENDS ${transpile_inputs} transpiler
    OUTPUT
        ${json_header}
        ${json_source}
        ${json_db_init}
    COMMENT "Running transpiler to generate JSON data for templates."
)

add_custom_command(
    COMMAND stst
        -o  ${stst_output_hpp}
        -t  ${stst_templates_dir}
            generate_header.header
            ${json_header}
    DEPENDS ${json_header}
            ${stst_templates_dir}/generate_header.stg
    OUTPUT  ${stst_output_hpp}
    COMMENT "Running StringTemplate Standalone Tool (stst) to generate header (hpp)."
)

add_custom_command(
        COMMAND stst
            -o  ${stst_output_cpp}
            -t  ${stst_templates_dir}
                generate_source.source
                ${json_source}
        DEPENDS ${json_source}
                ${stst_templates_dir}/generate_source.stg
        OUTPUT  ${stst_output_cpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate source (cpp)."
)

add_custom_command(
        COMMAND stst
            -o  ${stst_output_sql}
            -t  ${stst_templates_dir}
                generate_db_init.database
                ${json_db_init}
        DEPENDS ${json_db_init}
                ${stst_templates_dir}/generate_db_init.stg
        OUTPUT ${stst_output_sql}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate database script (sql)."
)

add_custom_command(
        COMMAND sqlite3 ${sqlite_output_db} ".read ${stst_output_sql}"
        DEPENDS ${stst_output_sql}
        OUTPUT ${sqlite_output_db}
        #TARGET user_prog POST_BUILD # Alternative to add_custom_target, below.
        COMMENT "Running Sqlite3 to generate database file (db)."
)

# TODO:  What we might want to do is create a custom target (replacing
# this one) that depends upon both the database and the executable.
# that way we don't make either wait for the other to start building
# but still ensure that both are built before the user runs the program.
# Perhaps it should/could be a thin wrapper executable.
add_custom_target(
        prog_db ALL
        COMMENT "Dummy target to cause db to be generated."
        DEPENDS ${sqlite_output_db}
)
