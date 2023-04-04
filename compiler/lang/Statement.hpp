#pragma once

#include <variant>
#include <string>
#include <vector>
#include "Expression.hpp"

namespace knife::lang
{
    struct LetStatement
    {
        std::string name;
        std::string type;
        std::string init;
    };

    struct FreshStatement
    {
        std::string name;
        std::string type;
    };

    struct YieldStatement
    {
        lang::Expression result;
    };

    struct MemberStatement
    {
        std::string name;
        std::string type;
        std::vector<std::string> args;
    };

    using Statement = std::variant<
            LetStatement,
            FreshStatement,
            YieldStatement,
            MemberStatement>;
}