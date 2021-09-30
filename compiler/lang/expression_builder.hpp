#pragma  once

#include "Expression.hpp"

namespace knife::lang::expression_builder
{
    inline Expression lit_int(int const& value)
    {
        return Expression(std::make_unique<IntExpr>(value));
    }

    inline Expression id(std::string name)
    {
        return Expression(std::make_unique<IdExpr>(name));
    }

    template <typename... Ts>
    Expression call(std::string name, Ts... args)
    {
        return Expression(
                std::make_unique<CallExpr>(
                        CallExpr { name, {args...}} ));
    }

    inline Expression binop(std::string op, Expression lhs, Expression rhs)
    {
        return Expression(
                std::make_unique<BinOpExpr>(
                        op, lhs, rhs ));
    }
}
