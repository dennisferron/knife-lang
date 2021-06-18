#pragma once

#include "Relation.hpp"

#include <vector>

namespace lang
{

    struct Program
    {
        std::vector<lang::Relation> relations;
    };

}
