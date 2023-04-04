#include "boost/test/unit_test.hpp"
#include "Utility.hpp"
#include "knife-exports.hpp"
#include "lang/expression_builder.hpp"

using namespace knife;
using namespace knife::lang::expression_builder;

BOOST_AUTO_TEST_SUITE(ParsingTests)

    lang::Expression parse(std::string input)
    {
        KnifeParserContext* parser = create_string_parser(input);
        lang::Expression result = parser->parse_expression();
        delete parser;
        return result;
    }

    BOOST_AUTO_TEST_CASE(test1)
    {
        BOOST_TEST( parse("42") == lit_int(42) );
    }

    BOOST_AUTO_TEST_CASE(test2)
    {
        BOOST_TEST( parse("A(x) & B(y)") ==
            binop("&", call("A", id("x")), call("B", id("y")))
        );
    }

BOOST_AUTO_TEST_SUITE_END()