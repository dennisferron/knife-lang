#pragma once

#include <vector>
#include <algorithm>
#include <variant>
#include <optional>
#include <ostream>
#include <sstream>
#include <map>
#include <iostream>
#include <memory>

template <typename T>
struct lvar
{
    int index = 0;
    char const* name = nullptr;
};

template <typename T>
std::ostream& operator <<(std::ostream& os, lvar<T> const& x)
{
    if (x.name)
        os << x.name;
    else
        os << "_";

    os << "." << x.index;

    return os;
}

template <typename T>
bool operator ==(lvar<T> const& x, lvar<T> const& y)
{
    // Note: names may not always match when indicies match,
    // because sometimes lvar's are recovered from bare ints.
    return x.index == y.index;
}

using Term = std::variant<void const*, int>;


struct Binding
{
    int from;
    Term to;
};

// TODO:  Potentially rename Environment to Subst
class Environment;

// Although this method doesn't use the environment param, specializations
// of unify_val for certain types might generate additional substitutions.
// This can happen when T is a struct which may contain ungrounded variables.
template <typename T>
bool unify_value(T const* x, T const* y, Environment& env)
{
    return *x == *y;
}

// Likewise we may specialize this function for structs with embedded vars.
template <typename T>
bool occurs(int v, T const* x)
{
    return false;
}

struct ValuePrinterBase
{
    virtual std::ostream& print(std::ostream& os, void const* p) const = 0;
};

template <typename T>
struct ValuePrinter : ValuePrinterBase
{
    std::ostream& print(std::ostream& os, void const* p) const override
    {
        if (p == nullptr)
            return os << "null";
        else
            return os << p << " " << *static_cast<T const*>(p);
    }
};

class Environment
{
private:
    std::vector<Binding> bindings;
    std::map<int, std::unique_ptr<ValuePrinterBase>> printers;

    Term walk(Term const& v) const
    {
        if (int const* from_var = std::get_if<int>(&v))
        {
            for (auto iter = bindings.crbegin(); iter != bindings.crend(); iter++)
            {
                if (iter->from == *from_var)
                    return walk(iter->to);
            }
            return v; // v is unbound lvar
        }
        else
        {
            return v;  // v is a value;
        }
    }

    friend struct UnifyVisitor;

    template <typename T>
    struct UnifyVisitor
    {
        Environment& env;

        bool operator()(int u, int v) const
        {
            if (u==v)  // if u and v are same variable
                return true;  // succeed w/o adding binding
            else
            {
                env.bindings.push_back({u, v});
                return true;
            }
        }

        bool operator()(int u, void const* v) const
        {
            if (occurs(u, v))
                return false;
            else
            {
                std::cout << " add binding {" << u << ","
                    << v << " " << *static_cast<T const*>(v) << "}\n";
                env.bindings.push_back({u, v});
                env.printers[u] = std::make_unique<ValuePrinter<T>>();
                return true;
            }
        }

        bool operator()(void const* u, int v) const
        {
            return operator()(v, u);  // transpose the args
        }

        bool operator()(void const* u, void const* v) const
        {
            if (u==v)  // if u and v point to same object
                return true;
            else
            {
                auto chkpt = env.save_checkpoint();

                T const* u_ = static_cast<T const*>(u);
                T const* v_ = static_cast<T const*>(v);

                std::cout << "unify_value(" << u << " " << *u_ << ", "
                    << v << " " << *v_ << ") == ";

                if (unify_value(u_, v_, env))
                {
                    std::cout << "success\n";
                    return true;
                }
                else
                {
                    std::cout << "failure\n";
                    env.revert_to_checkpoint(chkpt);
                    return false;
                }
            }
        }
    };

    template <typename T>
    bool unify(Term const& u, Term const& v)
    {
        Term u_ = walk(u);
        Term v_ = walk(v);

        UnifyVisitor<T> visitor { *this };
        return std::visit(visitor, u_, v_);
    }

    std::ostream& print(std::ostream& os, std::optional<std::size_t> cursor = std::nullopt) const
    {
        for (std::size_t i=0; i<bindings.size(); i++)
        {
            if (i == cursor)
                os << "(<-- here) ";

            auto const& b = bindings[i];

            os << "{" << b.from << ",";

            if (auto p = std::get_if<void const*>(&b.to))
            {
                // debugging
                auto x = lookup(b.from);
                if (!x)
                    throw std::logic_error("var not found or not a value");
                else
                {
                    auto p2 = *x;
                    if (p2 != *p)
                        throw std::logic_error("ptr to val does not match");
                }

                printers.at(b.from)->print(os, *p);
            }
            else  // b.to is a var
                os << std::get<int>(b.to);
            os << "} ";
        }
        return os;
    }

public:

    friend std::ostream& operator <<(std::ostream& os, Environment const& env)
    {
        return env.print(os);
    }

    template <typename T>
    std::optional<T const*> lookup(lvar<T> var) const
    {
        Term t = walk(var.index);
        if (auto p = std::get_if<void const*>(&t))
            return static_cast<T const*>(*p);
        else
            return std::nullopt;
    }

    std::optional<void const*> lookup(int var) const
    {
        Term t = walk(var);
        if (auto p = std::get_if<void const*>(&t))
            return *p;
        else
            return std::nullopt;
    }

    template <typename T>
    bool eq(lvar<T> var1, lvar<T> var2)
    {
        return unify<T>(var1.index, var2.index);
    }

    template <typename T>
    bool eq(lvar<T> var, T const* val)
    {
        return unify<T>(var.index, val);
    }

    template <typename T>
    bool eq(T const* val, lvar<T> var)
    {
        return unify<T>(var.index, val);
    }

    template <typename T>
    bool eq(T const* val1, T const* val2)
    {
        return unify<T>(val1, val2);
    }

    template <typename T>
    void set_var(lvar<T> var, T const* val)
    {
        if (!unify<T>(var.index, val))
        {
            std::stringstream msg;
            msg << "Failed to unify logic variable " << var
                << " with value " << (void const*) val << ".";
            throw std::runtime_error(msg.str());
        }
    }

    std::size_t save_checkpoint() const
    {
        std::cout << "save_checkpoint() == " << bindings.size()
            << " Environment: " << *this << "\n";
        return bindings.size();
    }

    void revert_to_checkpoint(std::size_t pos)
    {
        std::cout << "revert_to_checkpoint(" << bindings.size() << ") "
           << " Environment: ";
        print(std::cout, pos);
        std::cout << "\n";

        if (pos > bindings.size())
            throw std::out_of_range("Bindings is already shorter than checkpoint.");

        bindings.resize(pos);
    }
};

