#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>
#include <ParserRuleContext.h>

namespace lang
{
    class Expression
    {
    public:
        inline virtual ~Expression() {}
        virtual std::ostream& print(std::ostream& os) const = 0;
        virtual bool equals(Expression const& that) const = 0;
    };

    inline std::ostream& operator <<(std::ostream& os, Expression const& expr)
    {
        return expr.print(os);
    }

    inline bool operator ==(Expression const& x, Expression const& y)
    {
        return x.equals(y);
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

        bool equals(const Expression& that) const override
        {
            if (auto p = dynamic_cast<IntExpr const*>(&that))
                return p->value == this->value;
            else
                return false;
        }
    };

    class QuasiquoteExpr : public Expression
    {
    public:
        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "QuasiquoteExpr";
        }

        bool equals(const Expression& that) const override
        {
            throw std::logic_error("Not implemented: QuasiquoteExpr::equals");
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

        bool equals(const Expression& that) const override
        {
            if (auto p = dynamic_cast<BinOpExpr const*>(&that))
            {
                return p->op == this->op
                    && *(p->lhs) == *(this->lhs)
                    && *(p->rhs) == *(this->rhs);
            }
            else
            {
                return false;
            }
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

        bool equals(const Expression& that) const override
        {
            if (auto p = dynamic_cast<CallExpr const*>(&that))
            {
                return p->method_name == this->method_name
                    && p->params == this->params;
            }
            else
            {
                return false;
            }
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

        bool equals(const Expression& that) const override
        {
            if (auto p = dynamic_cast<IdExpr const*>(&that))
            {
                return p->identifier == this->identifier;
            }
            else
            {
                return false;
            }
        }
    };
}
