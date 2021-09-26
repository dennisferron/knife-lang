#pragma once

#include "antlr4-runtime.h"
#include "KnifeLexer.h"
#include "KnifeParser.h"
#include "ParseListener.hpp"
#include "outp/Output.hpp"
#include "Transform.hpp"
#include "data/ParseLogger.hpp"

#include "KnifeParserContext.hpp"

namespace knife
{
    // We want to be warned about ambiguous parses while developing grammar,
    // so using DiagnosticErrorListener in place of BaseErrorListener.
    class CerrErrorListener : public antlr4::DiagnosticErrorListener
    {
        virtual void syntaxError(
            antlr4::Recognizer* recognizer,
            antlr4::Token* offendingSymbol,
            size_t line,
            size_t charPositionInLine,
            const std::string& msg,
            std::exception_ptr e) override
        {
            std::cerr << "Error on Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ")";
        }
    };

    class KnifeParserContextImpl : public KnifeParserContext
    {
    private:
        data::ParseLogger* logger;
        KnifeLexer knife_lexer;
        std::unique_ptr<KnifeParser> knife_parser;

        void init(std::string input_file);

    public:
        KnifeParserContextImpl(data::ParseLogger* logger, antlr4::CharStream* input);
        lang::Program parse() override;
        void log_token_names() override;
    };

}