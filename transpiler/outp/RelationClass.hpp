#pragma once

#include "StepCase.hpp"

namespace outp
{
    struct RelationClass
    {
        std::string name;
        std::vector<StepCase> step_cases;
    };
}
