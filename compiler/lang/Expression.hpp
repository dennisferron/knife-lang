#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>
#include <ParserRuleContext.h>

namespace knife::lang
{
    class ExprBase
    {
    public:
        inline virtual ~ExprBase() {}
        virtual std::ostream& print(std::ostream& os) const = 0;
        virtual bool equals(ExprBase const& that) const = 0;
        virtual ExprBase const* clone() const = 0;
    };

    class Expression
    {
    private:
        std::unique_ptr<ExprBase const> expr;

    public:
        Expression()
            : expr(nullptr) {}

        Expression(std::unique_ptr<ExprBase const> expr)
            : expr(std::move(expr)) {}

        Expression(Expression const& that)
            : expr(that.expr ? that.expr->clone() : nullptr)
        {
        }
        
        Expression& operator=(Expression const& that)
        {
            if (this == &that)
                return *this;
            else
            {
                expr.reset(that.expr ? that.expr->clone() : nullptr);
                return *this;
            }
        }

        friend std::ostream& operator <<(std::ostream& os, Expression const& x)
        {
            if (x.expr)
                return x.expr->print(os);
            else
                return os << "<null>";
        }

        friend bool operator ==(Expression const& x, Expression const& y)
        {
            if (x.expr && y.expr) // if both non-null
                return x.expr->equals(*y.expr); // then deep compare
            else    // else at least one is null
                return x.expr == y.expr; // equal if both are null
        }
    };

    class IntExpr : public ExprBase
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

        bool equals(const ExprBase& that) const override
        {
            if (auto p = dynamic_cast<IntExpr const*>(&that))
                return p->value == this->value;
            else
                return false;
        }

        const ExprBase* clone() const override
        {
            return new IntExpr(value);
        }
    };

    class QuasiquoteExpr : public ExprBase
    {
    public:
        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "QuasiquoteExpr";
        }

        bool equals(const ExprBase& that) const override
        {
            throw std::logic_error("Not implemented: QuasiquoteExpr::equals");
        }

        const ExprBase* clone() const override
        {
            return new QuasiquoteExpr();
        }
    };

    class BinOpExpr : public ExprBase
    {
    private:
        std::string op;
        Expression lhs;
        Expression rhs;

    public:
        BinOpExpr(const std::string& op,
                  Expression const& lhs,
                  Expression const& rhs) :
        op(op), lhs(lhs), rhs(rhs)
        {}

        virtual std::ostream& print(std::ostream& os) const override
        {
            return os << "(BinOpExpr "
                << op << " "
                << lhs << " "
                << rhs << ")";
        }

        bool equals(const ExprBase& that) const override
        {
            if (auto p = dynamic_cast<BinOpExpr const*>(&that))
            {
                return p->op == this->op
                    && p->lhs == this->lhs
                    && p->rhs == this->rhs;
            }
            else
            {
                return false;
            }
        }

        const ExprBase* clone() const override
        {
            return new BinOpExpr(op, lhs, rhs);
        }
    };

    class CallExpr : public ExprBase
    {
    private:
        std::string method_name;
        std::vector<Expression> params;
    public:
        CallExpr(const std::string& methodName, const std::vector<Expression>& params)
            : method_name(methodName), params(params) {}

        virtual std::ostream& print(std::ostream& os) const override
        {
            os << "(CallExpr "
            << method_name
            << "\\" << params.size();

            for (auto p : params)
                os << " " << p;

            return os << ")";
        }

        bool equals(const ExprBase& that) const override
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

        const ExprBase* clone() const override
        {
            return new CallExpr(method_name, params);
        }
    };

    class IdExpr : public ExprBase
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

        bool equals(const ExprBase& that) const override
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

        const ExprBase* clone() const override
        {
            return new IdExpr(identifier);
        }
    };
}
