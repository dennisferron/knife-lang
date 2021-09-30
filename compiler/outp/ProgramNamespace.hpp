#pragma once

#include "RelationClass.hpp"

namespace knife::outp
{
    struct ProgramNamespace
    {
        std::string name;
        std::vector<RelationClass> classes;
    };
}
