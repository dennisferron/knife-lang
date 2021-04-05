#pragma once

#include <vector>
#include <algorithm>

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

struct Binding
{
    virtual void const* lookup(int v) const = 0;
};

class Environment
{
private:
    std::vector<Binding const*> bindings;

public:

    void add(Binding const* value)
    {
        bindings.push_back(value);
    }

    void remove(Binding const* value)
    {
        auto& b = bindings;

        b.erase(
            std::remove(b.begin(), b.end(), value),
            b.end());
    }

    void const* lookup(int v) const
    {
        for (auto b : bindings)
        {
            void const* p = b->lookup(v);
            if (p)
                return p;
        }
        return nullptr;
    }

    template <typename T>
    T const* lookup(lvar<T> var)
    {
        return reinterpret_cast<T const*>(lookup(var.index));
    }
};

