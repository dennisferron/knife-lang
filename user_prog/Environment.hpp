#pragma once

template <typename T>
struct lvar
{
    int index = 0;
    char const* name = nullptr;
};

template <typename T>
bool operator ==(lvar<T> const& x, lvar<T> const& y)
{
    // TODO: sanity check names match if indices match.
    return x.index == y.index;
}

struct SubstBase
{
    virtual void* lookup(int v) const = 0;
};

template <typename T>
struct Subst
{
    T* relation;
    typename T::Result result;

    void* lookup(int v) const
    {
        return relation->reflect(result, result);
    }
};

class Environment
{
private:
    Environment const* inherits = nullptr;
    SubstBase const* subst = nullptr;

    Environment(Environment const* inherits, SubstBase const* subst)
            : inherits(inherits), subst(subst) {}

    void* lookup(int v) const
    {
        void* p;

        if (subst && (p = subst->lookup(v)))
            return p;
        else if (inherits && (p = inherits->lookup(v)))
            return p;
        else
            return nullptr;
    }

public:
    Environment() {}

    Environment const* extend(SubstBase const* subst) const
    {
        return new Environment(this, subst);
    }

    template <typename T>
    T* lookup(lvar<T> var) const
    {
        return nullptr;
    }
};

