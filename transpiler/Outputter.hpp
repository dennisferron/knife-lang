#pragma once

#include "outp/ProgramNamespace.hpp"

#include <ostream>

namespace knife
{
    class Outputter
    {
    public:
        virtual ~Outputter() {}
        virtual void output_header(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) = 0;
        virtual void output_source(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) = 0;
        virtual void output_db_init(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) = 0;
    };

}