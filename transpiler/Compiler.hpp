#pragma once

#include "lang/Program.hpp"
#include "outp/ProgramNamespace.hpp"

#include <ostream>

namespace knife
{
    class Compiler
    {
    public:
        virtual outp::ProgramNamespace transform(lang::Program const& program) = 0;
    };
}
