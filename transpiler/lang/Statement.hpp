#pragma once

#include <variant>

namespace lang
{
    class LetStatement
    {
    };

    class FreshStatement
    {
    };

    class YieldStatement
    {
    };

    using Statement = std::variant<
            LetStatement,
            FreshStatement,
            YieldStatement>;
}