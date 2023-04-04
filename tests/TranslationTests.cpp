#include "boost/test/unit_test.hpp"
#include "Utility.hpp"
#include "knife-exports.hpp"
#include "lang/expression_builder.hpp"
#include "lang/Relation.hpp"
#include "outp/RelationClass.hpp"

using namespace knife;
using namespace knife::lang::expression_builder;

BOOST_AUTO_TEST_SUITE(TranslationTests)

    BOOST_AUTO_TEST_CASE(test_conj)
    {
//        lang::YieldStatement stmt
//        {
//            binop("&", id("a"), id("b"))
//        };
//        outp::RelationClass actual = transform(rel);
//        BOOST_TEST( parse("A(x) & B(y)") ==
//            binop("&", call("A", id("x")), call("B", id("y")))
//        );
    }

BOOST_AUTO_TEST_SUITE_END()