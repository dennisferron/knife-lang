#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>

namespace lang
{
    class Expression
    {
    public:
        virtual ~Expression() = 0;
        virtual std::ostream& print(std::ostream& os) const = 0;
    };

    inline std::ostream& operator <<(std::ostream& os, Expression const& expr)
    {
        return expr.print(os);
    }

    class IntExpr : public Expression
    {
    private:
        int value;

    public:
        IntExpr(int value) :
                value(value)
        {}

        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "(IntExpr "
                << value << ")";
        }
    };

    class QuasiquoteExpr : public Expression
    {
        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "QuasiquoteExpr";
        }
    };

    class BinOpExpr : public Expression
    {
    private:
        std::string op;
        Expression const* lhs;
        Expression const* rhs;

    public:
        BinOpExpr(const std::string& op, const Expression* lhs, const Expression* rhs) :
        op(op), lhs(lhs), rhs(rhs)
        {}

        virtual ~BinOpExpr()
        {
            delete lhs;
            delete rhs;
        }

        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "(BinOpExpr "
            << op << " " << *lhs << " "
            << *rhs << ")";
        }
    };

    class CallExpr : public Expression
    {
    private:
        std::string method_name;
        std::vector<Expression const*> params;
    public:
        CallExpr(const std::string& methodName, const std::vector<const Expression*>& params) :
        method_name(methodName), params(params)
        {}

        virtual ~CallExpr()
        {
            for (auto p : params)
                delete p;
        }

        virtual std::ostream& print(std::ostream& os) const override
        {
            os << "(CallExpr "
            << method_name
            << "\\" << params.size();

            for (auto p : params)
                os << " " << *p;

            return os << ")";
        }
    };

    class IdExpr : public Expression
    {
    private:
        std::string identifier;

    public:
        IdExpr(const std::string& identifier)
         : identifier(identifier)
        {}

        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "(IdExpr "
            << identifier << ")";
        }
    };
}
