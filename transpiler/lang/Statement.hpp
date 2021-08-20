#pragma once

#include <variant>
#include <string>
#include <vector>

namespace lang
{
    struct LetStatement
    {
    };

    struct FreshStatement
    {
    };

    struct YieldStatement
    {
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