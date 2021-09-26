#include "boost/test/unit_test.hpp"
#include "Utility.hpp"
#include "knife-exports.hpp"

BOOST_AUTO_TEST_SUITE(CompilationTests)

    BOOST_AUTO_TEST_CASE(test1)
    {
        auto parser = knife::create_string_parser(
        R"(
            A(x) & B(y)
        )");
//        BOOST_TEST( false );
//        BOOST_TEST( parser->parse_expression().equals(
//                lang::IntExpr {42}) );
    }

BOOST_AUTO_TEST_SUITE_END()