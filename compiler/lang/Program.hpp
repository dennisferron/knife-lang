#pragma once

#include "Relation.hpp"

#include <vector>

namespace knife::lang
{

    struct Program
    {
        std::vector<lang::Relation> relations;

        lang::Relation& last_relation();
    };

}
