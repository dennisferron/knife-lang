cmake_minimum_required(VERSION 3.17)
project(antlr4_demo)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(antlr4-runtime)

set(ANTLR_COMMAND java org.antlr.v4.Tool)
set(ANTLR_OPTIONS -Dlanguage=Cpp -o ${CMAKE_CURRENT_BINARY_DIR})

set(EXPR_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/Expr.g4)
set(EXPR_GEN_INPUTS Expr.g4)
set(EXPR_GEN_OUTPUTS ExprBaseListener.cpp
        ExprBaseListener.h
        ExprLexer.cpp
        ExprLexer.h
        ExprListener.cpp
        ExprListener.h
        ExprParser.cpp
        ExprParser.h)

add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${EXPR_GRAMMAR}
        DEPENDS ${EXPR_GEN_INPUTS}
        OUTPUT ${EXPR_GEN_OUTPUTS}
        COMMENT "Running ANTLR4 (Java) to generate the Expr parser."
)

add_executable(antlr4_demo main.cpp ${EXPR_GEN_OUTPUTS} ${SUBST_GEN_OUTPUTS})
target_link_libraries(antlr4_demo antlr4_static)

target_include_directories(antlr4_demo
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        )

set(extra_static_compile_flags "-DUSE_UTF8_INSTEAD_OF_CODECVT -DANTLR4CPP_STATIC")

set_target_properties(antlr4_demo PROPERTIES COMPILE_FLAGS ${extra_static_compile_flags})

set(transpile_inputs ${CMAKE_CURRENT_SOURCE_DIR}/samples/t.expr)
set(transpile_output output.json)

add_custom_command(
        TARGET antlr4_demo POST_BUILD
        COMMAND antlr4_demo -o ${transpile_output} -e ${transpile_inputs}
        DEPENDS ${transpile_inputs}
        #OUTPUT ${transpile_output}
        COMMENT "Running transpiler bootstrap step."
)

set(stst_templates_dir ${CMAKE_CURRENT_SOURCE_DIR}/templates)
set(stst_json ${transpile_output})

set(stst_template_file ExprProg.stg)
set(stst_inputs ${stst_json} ExprProg.stg)

set(stst_template_hpp ExprProg.header)
set(stst_output_hpp prog.hpp)
add_custom_command(
        TARGET antlr4_demo POST_BUILD
        COMMAND stst
            -o  ${stst_output_hpp}
            -t  ${stst_templates_dir}
                ${stst_template_hpp}
                ${stst_json}
        DEPENDS ${stst_inputs}
        #OUTPUT ${stst_output_hpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate header (hpp)."
)

set(stst_template_cpp ExprProg.source)
set(stst_output_cpp prog.cpp)
add_custom_command(
        TARGET antlr4_demo POST_BUILD
        COMMAND stst
            -o  ${stst_output_cpp}
            -t  ${stst_templates_dir}
                ${stst_template_cpp}
                ${stst_json}
        DEPENDS ${stst_inputs}
        #OUTPUT ${stst_output_cpp}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate source (cpp)."
)

set(stst_template_sql SqlScripts.database)
set(stst_output_sql prog.sql)
add_custom_command(
        TARGET antlr4_demo POST_BUILD
        COMMAND stst
        -o  ${stst_output_sql}
        -t  ${stst_templates_dir}
        ${stst_template_sql}
        ${stst_json}
        DEPENDS SqlScripts.stg ${transpile_output}
        #OUTPUT ${stst_output_sql}
        COMMENT "Running StringTemplate Standalone Tool (stst) to generate database script (sql)."
)

# TODO:  Move the transpiler to a subfolder
# TODO:  Build the stage_1 transpiler from stage_0 output.
# TODO:  Add an example using the stage_1 tool to build user program.
