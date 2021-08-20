#pragma once

#include "StepCase.hpp"

namespace outp
{
    struct ParamVar
    {
        std::string var_name;
        std::string type;
    };

    struct ResultVar
    {
        std::string var_name;
        std::string type;
    };

    struct RelationClass
    {
        std::string name;
        std::vector<ParamVar> param_lvars;
        std::vector<ResultVar> result_vars;
        std::vector<FreshVar> fresh_lvars;
        std::vector<SubRel> members;
        std::vector<StepCase> step_cases;
    };
}
