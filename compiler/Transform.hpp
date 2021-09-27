#pragma once

#include "lang/Program.hpp"
#include "outp/ProgramNamespace.hpp"

namespace knife
{
    outp::ProgramNamespace transform(lang::Program const& inp_prog);
    outp::RelationClass transform(lang::Relation const& inp_rel);
}
