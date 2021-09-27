#pragma once

#include "RelationClass.hpp"

namespace outp
{
    struct ProgramNamespace
    {
        std::string name;
        std::vector<RelationClass> classes;
    };
}
