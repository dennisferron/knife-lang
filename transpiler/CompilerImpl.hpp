#pragma once

#include "antlr4-runtime.h"
#include "KnifeLexer.h"
#include "KnifeParser.h"
#include "ParseListener.hpp"
#include "outp/Output.hpp"
#include "Transform.hpp"
#include "data/ParseLogger.hpp"

#include "Compiler.hpp"

namespace knife
{
    class CompilerImpl : public Compiler
    {
    private:
        data::ParseLogger* logger;

    public:
        CompilerImpl(data::ParseLogger* logger);
        outp::ProgramNamespace transform(lang::Program const& program) override;
    };
}

