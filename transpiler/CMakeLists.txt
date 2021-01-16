cmake_minimum_required(VERSION 3.17)
project(transpiler)

set(CMAKE_CXX_STANDARD 20)

set(ANTLR_COMMAND java org.antlr.v4.Tool)
set(ANTLR_OPTIONS -Dlanguage=Cpp -o ${CMAKE_CURRENT_BINARY_DIR})

set(EXPR_LEXER_OUTPUTS ExprLexer.cpp ExprLexer.h)
add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/ExprLexer.g4
        DEPENDS ExprLexer.g4
        OUTPUT ${EXPR_LEXER_OUTPUTS} ExprLexer.tokens
        COMMENT "Running ANTLR4 (Java) to generate the Expr lexer."
)

set(EXPR_PARSER_OUTPUTS
        ExprParserBaseListener.cpp
        ExprParserBaseListener.h
        ExprParserListener.cpp
        ExprParserListener.h
        ExprParser.cpp
        ExprParser.h
)
add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/ExprParser.g4
        DEPENDS ExprParser.g4 ExprLexer.tokens
        OUTPUT ${EXPR_PARSER_OUTPUTS}
        COMMENT "Running ANTLR4 (Java) to generate the Expr parser."
)

add_executable(transpiler main.cpp ${EXPR_LEXER_OUTPUTS} ${EXPR_PARSER_OUTPUTS} ${SUBST_GEN_OUTPUTS})
target_link_libraries(transpiler antlr4_static)

target_include_directories(transpiler
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        )

set(extra_static_compile_flags "-DUSE_UTF8_INSTEAD_OF_CODECVT -DANTLR4CPP_STATIC")

set_target_properties(transpiler PROPERTIES COMPILE_FLAGS ${extra_static_compile_flags})
