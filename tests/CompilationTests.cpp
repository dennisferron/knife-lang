#include "boost/test/unit_test.hpp"
#include "Utility.hpp"
#include "knife-exports.hpp"

BOOST_AUTO_TEST_SUITE(CompilationTests)

//    auto parse(std::string input)
//    {
//        auto parser = knife::create_string_parser(input);
//        auto result = parser->parse_expression();
//    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        auto parser = knife::create_string_parser(
        R"(
            42
        )");
        BOOST_TEST( parser->parse_expression() ==
        lang::Expression(std::make_unique<lang::IntExpr>(42)) );
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
    auto parser = knife::create_string_parser(
            R"(
            A(x) & B(y)
            )");
    BOOST_TEST( parser->parse_expression() ==
            lang::Expression(std::make_unique<lang::IntExpr>(42)) );
    }

BOOST_AUTO_TEST_SUITE_END()