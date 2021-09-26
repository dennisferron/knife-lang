#pragma once

#include "lang/Program.hpp"
#include "lang/Expression.hpp"

namespace knife::data
{
    class ParseLogger;
}

namespace knife
{
    class KnifeParserContext
    {
    public:
        virtual ~KnifeParserContext() {}
        // TODO:  might need to change lang::Program to TranslationUnit
        virtual lang::Program parse_program() = 0;
        virtual lang::Expression const& parse_expression() = 0;
        virtual void log_token_names() = 0;
    };
}
