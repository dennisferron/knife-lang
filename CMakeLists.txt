cmake_minimum_required(VERSION 3.17)
project(antlr4_demo)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(antlr4-runtime)

set(ANTLR_COMMAND java org.antlr.v4.Tool)
set(ANTLR_OPTIONS -Dlanguage=Cpp -o ${CMAKE_CURRENT_BINARY_DIR}/parser)
set(ANTLR_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/Expr.g4)

set(EXPR_GEN_INPUTS data/template.inl Expr.g4)
set(EXPR_GEN_OUTPUTS ExprBaseListener.cpp
        ExprBaseListener.h
        ExprLexer.cpp
        ExprLexer.h
        ExprListener.cpp
        ExprListener.h
        ExprParser.cpp
        ExprParser.h)

add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${ANTLR_GRAMMAR}
        DEPENDS ${EXPR_GEN_INPUTS}
        OUTPUT ${EXPR_GEN_OUTPUTS}
        COMMENT "Running ANTLR4 (Java) to generate a parser."
)

add_executable(antlr4_demo main.cpp ${EXPR_GEN_OUTPUTS})
target_link_libraries(antlr4_demo antlr4_static)

target_include_directories(antlr4_demo
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        )

set(extra_static_compile_flags "-DUSE_UTF8_INSTEAD_OF_CODECVT -DANTLR4CPP_STATIC")

set_target_properties(antlr4_demo PROPERTIES COMPILE_FLAGS ${extra_static_compile_flags})

set(TRANSPILE_INPUTS data/t.expr data/template.inl)
set(TRANSPILE_OUTPUTS foo.xyz)

add_custom_command(
        TARGET antlr4_demo POST_BUILD
        COMMAND antlr4_demo -o ${TRANSPILE_OUTPUTS} -e data/t.expr -t data/template.inl
        DEPENDS ${TRANSPILE_INPUTS}
        #OUTPUT ${TRANSPILE_OUTPUTS}
        COMMENT "Running transpiler bootstrap step."
)

# TODO:  Create a grammar for the template language
# TODO:  Move the transpiler to a subfolder
# TODO:  Build the stage_1 transpiler from stage_0 output.
# TODO:  Add an example using the stage_1 tool to build user program.
