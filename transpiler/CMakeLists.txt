cmake_minimum_required(VERSION 3.17)
project(transpiler)

set(CMAKE_CXX_STANDARD 20)

set(ANTLR_COMMAND java org.antlr.v4.Tool)
set(ANTLR_OPTIONS -Dlanguage=Cpp -o ${CMAKE_CURRENT_BINARY_DIR})

set(EXPR_LEXER_OUTPUTS KnifeLexer.cpp KnifeLexer.h)
add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/KnifeLexer.g4
        DEPENDS KnifeLexer.g4
        OUTPUT ${EXPR_LEXER_OUTPUTS} KnifeLexer.tokens
        COMMENT "Running ANTLR4 (Java) to generate the Expr lexer."
)


set(EXPR_PARSER_OUTPUTS
        KnifeParserBaseListener.cpp
        KnifeParserBaseListener.h
        KnifeParserListener.cpp
        KnifeParserListener.h
        KnifeParser.cpp
        KnifeParser.h
)
add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/KnifeParser.g4
        DEPENDS KnifeParser.g4 KnifeLexer.tokens
        OUTPUT ${EXPR_PARSER_OUTPUTS}
        COMMENT "Running ANTLR4 (Java) to generate the Expr parser."
)

add_executable(transpiler main.cpp ${EXPR_LEXER_OUTPUTS} ${EXPR_PARSER_OUTPUTS} ${SUBST_GEN_OUTPUTS} ParseListener.cpp ParseListener.hpp lang/Relation.cpp lang/Relation.hpp lang/Program.cpp lang/Program.hpp Json.cpp Json.hpp outp/Output.cpp outp/Output.hpp lang/Statement.cpp lang/Statement.hpp outp/StepCase.cpp outp/StepCase.hpp outp/RelationClass.cpp outp/RelationClass.hpp outp/ProgramNamespace.cpp outp/ProgramNamespace.hpp Transform.cpp Transform.hpp lang/Expression.cpp lang/Expression.hpp data/LogDatabase.cpp data/LogDatabase.hpp)
target_link_libraries(transpiler antlr4_static sqlite_lib)

target_include_directories(transpiler
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        )

set(extra_static_compile_flags "-DUSE_UTF8_INSTEAD_OF_CODECVT -DANTLR4CPP_STATIC")

set_target_properties(transpiler PROPERTIES COMPILE_FLAGS ${extra_static_compile_flags})

set(ANTLR_JAVA_OPTIONS -o ${CMAKE_CURRENT_BINARY_DIR})

# In order to run the ANTLR4 TestRig, we need to generate and compile
# a Java version of the lexer and parser.  I'm taking some shortcuts
# with the dependencies between the following commands by choosing a
# single "representative" file among the many which are generated and
# pretending that is the (only) interdependency.  Since ANTLR4 generates
# all the relevant files as a set (e.g. KnifeLexer.java and KnifeLexer.tokens,
# or KnifeParser.java with ExprBaseListener.java, etc.), it should still
# lead to the same commands being run as necessary.

add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_JAVA_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/KnifeLexer.g4
        DEPENDS KnifeLexer.g4
        OUTPUT KnifeLexer.java #KnifeLexer.tokens (also generated)
        COMMENT "Running ANTLR4 (Java) to generate Java Expr lexer."
)

# We want the following command to depend upon the former because the
# file KnifeLexer.tokens is required as an input to parser generation.
# However since we previously declared KnifeLexer.tokens as a target of
# generating the C++ lexer, so also making it a target of the
# Java version of the lexer generation command would override that.
# So instead we pretend to depend on KnifeLexer.java as a stand-in.

add_custom_command(
        COMMAND ${ANTLR_COMMAND} ${ANTLR_JAVA_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/KnifeParser.g4
        DEPENDS KnifeParser.g4 KnifeLexer.java #force generate KnifeLexer.tokens
        OUTPUT KnifeParser.java
        COMMENT "Running ANTLR4 (Java) to generate Java Expr parser."
)

# Again, a shortcut:

add_custom_command(
        COMMAND javac Expr*.java
        DEPENDS KnifeParser.java KnifeLexer.java
        OUTPUT KnifeParser.class
        COMMENT "Running Java compiler to compile Java version of Lexer and Parser."
)

set(GRUN_COMMAND java org.antlr.v4.gui.TestRig)
set(GRUN_OPTIONS Expr prog)
set(GRUN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../user_prog/examples/ancestor.knf)

add_custom_target(
        grun_tree
        COMMAND ${GRUN_COMMAND} ${GRUN_OPTIONS} -tree ${GRUN_INPUT}
        DEPENDS ${GRUN_INPUT} KnifeParser.class
        COMMENT "Running ANTLR4 TestRig (Java) to debug the Java grammar."
)

add_custom_target(
        grun_gui
        COMMAND ${GRUN_COMMAND} ${GRUN_OPTIONS} -gui ${GRUN_INPUT}
        DEPENDS ${GRUN_INPUT} KnifeParser.class
        COMMENT "Running ANTLR4 TestRig (Java) to debug the Java grammar."
)

add_custom_target(
        grun_tokens
        COMMAND ${GRUN_COMMAND} ${GRUN_OPTIONS} -tokens ${GRUN_INPUT}
        DEPENDS ${GRUN_INPUT} KnifeParser.class
        COMMENT "Running ANTLR4 TestRig (Java) to debug the Java grammar."
)
