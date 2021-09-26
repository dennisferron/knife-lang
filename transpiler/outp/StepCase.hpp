#pragma once

#include <variant>
#include <vector>
#include <string>

namespace outp
{
    struct Case_SetResult
    {
        std::vector<std::string> result;
    };

    struct SubRel
    {
        std::string rel_name;
        std::string rel_type;
        std::vector<std::string> rel_args;
    };

    struct FreshVar
    {
        std::string var_name;
        std::string type;
        std::string comment;
    };

    struct Case_InitSubRel
    {
        std::vector<SubRel> prev_rels;
        std::vector<FreshVar> fresh_vars;
        std::vector<SubRel> init_rels;
    };

    using CaseTemplate = std::variant<
            Case_SetResult,
            Case_InitSubRel
            >;

    struct StepCase
    {
        int number;
        CaseTemplate case_template;
    };
}
