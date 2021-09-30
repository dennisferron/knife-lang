#include "OutputterImpl.hpp"
#include "outp/Output.hpp"

using namespace knife;
using namespace knife::outp;

void OutputterImpl::output_header(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name)
{
    OutputHeader(os, output_base_name).write(prog_ns);
}

void OutputterImpl::output_source(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name)
{
    OutputSource(os, output_base_name).write(prog_ns);
}

void OutputterImpl::output_db_init(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name)
{
    OutputDbInit(os, output_base_name).write(prog_ns);
}
