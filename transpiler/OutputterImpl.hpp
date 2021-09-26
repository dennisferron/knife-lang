#pragma once

#include <iosfwd>
#include "Outputter.hpp"
#include "outp/ProgramNamespace.hpp"

namespace knife
{
    class OutputterImpl : public Outputter
    {
    public:
        void output_header(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) override;

        void output_source(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) override;

        void output_db_init(std::ostream& os, outp::ProgramNamespace prog_ns, std::string output_base_name) override;
    };
}
