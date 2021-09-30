#pragma once

#include <variant>
#include <string>
#include <vector>

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