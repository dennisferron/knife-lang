#pragma once

#include "lang/Program.hpp"

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
        virtual lang::Program parse() = 0;
        virtual void log_token_names() = 0;
    };
}
