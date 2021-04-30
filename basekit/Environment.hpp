#pragma once

#include <vector>
#include <algorithm>
#include <variant>
#include <optional>
#include <ostream>
#include <sstream>
#include <map>

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

/*

Suppose the first value of the var is rel1.A = struct { X, 2 }
and the next value of the var is rel2.A = struct { 1, Y },
then to represent the unification of them we need a relation to
point to to get "X" and to get "Y".  We could let the relations
recursively return variables from their subterms, however what
if rel1 doesn't KNOW the "2" should be associated with "Y" and
rel2 doesn't KNOW the "1" should be associated with "X" (because
they each don't know the opposite's variables and the values 1
 or 2 are storage locations and only associated with vars positionally.)

The struct { _, _ } should overload unify for its particular type.
The value for vars X, Y should be able to point *into* the struct
 to address the values 1, 2.
So the struct { _, _ } should also implement positional access.
 The result of unify'ing the struct would be a list of subst'itutions
 with var(X)->&val(1), var(Y)->&val(2).
But we also need a way to tie these subst's to the current state, so
 that when rel2 is deleted (or reset), the subst's for X and Y go away.

 To recap, if either rel1's or rel2's A had been unbound, the first extension
 of the environment would have been to bind A to { X, 2 } or { 1, Y }
But since it was already bound, the subst's X->1 and Y->2 must be added
 to the environment so that they will be equal.
Later if the value of A is looked up, this subst must be applied to generate
 the result.  (This can easily be handled in unify, but should also be
 done when the user looks up a result.)
We need to associate a pointer to rel2 with the subst's of X and Y,
 so that when rel2 resets or clears, the environment can remove them.
This means a pointer to the "owning relation" or "asking relation"
 or "most recent relation" must be associated with the list of Subst's
 returned from unify.  (It only need be passed to unify if that pointer
 is part of class Subst.  It could instead be associated with the entire
 list of substitutions by environment after the list is computed.)

 */

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

class Environment
{
private:
    std::vector<Binding> bindings;

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
                env.bindings.push_back({u, v});
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

                T const* u_ = reinterpret_cast<T const*>(u);
                T const* v_ = reinterpret_cast<T const*>(v);

                if (unify_value(u_, v_, env))
                    return true;
                else
                {
                    env.revert_to_checkpoint(chkpt);
                    return false;
                }
            }
        }
    };

public:

    template <typename T>
    std::optional<T const*> lookup(lvar<T> var) const
    {
        Term t = walk(var.index);
        if (auto p = std::get_if<void const*>(&t))
            return reinterpret_cast<T const*>(p);
        else
            return std::nullopt;
    }

    template <typename T>
    bool unify(Term const& u, Term const& v)
    {
        Term u_ = walk(u);
        Term v_ = walk(v);

        UnifyVisitor<T> visitor { *this };
        return std::visit(visitor, u_, v_);
    }

    template <typename T>
    bool eq(lvar<T> var, T const* val)
    {
        return unify<T>(var.index, val);
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
        return bindings.size();
    }

    void revert_to_checkpoint(std::size_t pos)
    {
        if (pos > bindings.size())
            throw std::out_of_range("Bindings is already shorter than checkpoint.");

        bindings.resize(pos);
    }
};

