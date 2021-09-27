#include "outp/ProgramNamespace.hpp"

#include "CompilerImpl.hpp"

#include "Transform.hpp"

using namespace knife;

CompilerImpl::CompilerImpl(data::ParseLogger* logger)
    : logger(logger)
{
}

outp::ProgramNamespace CompilerImpl::transform(lang::Program const& program)
{
    return knife::transform(program);
}
