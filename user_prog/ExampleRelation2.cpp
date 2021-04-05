#include "ExampleRelation2.hpp"

Parent::Parent(lvar<Person> p, lvar<Person> c)
    : p(p), c(c)
{
}

bool Parent::bind(Environment& env)
{
    auto e_p = env.lookup(p);
    auto e_c = env.lookup(c);

    bool compat  = (!e_p || *e_p == cached_result.p)
                && (!e_c || *e_c == cached_result.c);

    if (compat)
        env.add(this);

    return compat;
}

void const* Parent::lookup(int v) const
{
    if (v == p.index)
        return &cached_result.p;
    else if (v == c.index)
        return &cached_result.c;
    else
        return nullptr;
}

bool Parent::step(Environment& env, int& var_counter)
{
    switch (next_step)
    {
        case 0:
            next_step = 1;
            cached_result = Result { "alice", "bob" };
            if (bind(env))
                return true;
        case 1:
            next_step = 2;
            cached_result =  Result { "bob", "chris" };
            if (bind(env))
                return true;
        case 2:
            next_step = 3;
            cached_result =  Result { "chris", "debbie" };
            if (bind(env))
                return true;
        case 3:
            next_step = 4;
            cached_result =  Result { "debbie", "edith" };
            if (bind(env))
                return true;
        default:
            env.remove(this);
            next_step = -1;
            return false;
    }
}

Ancestor::Ancestor(lvar<Person> ans, lvar<Person> des) :
    ans(ans), des(des) {}

Ancestor::~Ancestor()
{
    delete ancestor_1;
    delete parent_1;
    delete parent_0;
}

void Ancestor::reset()
{
    next_step = 0;

    if (parent_0)
        parent_0->reset();

    if (parent_1)
        parent_1->reset();

    if (ancestor_1)
        ancestor_1->reset();
}

bool Ancestor::step(Environment& env, int& var_counter)
{
    switch (next_step)
    {
        case 0:
            parent_0 = new Parent(ans, des);
            next_step = 1;
        case 1:
            if (parent_0->step(env, var_counter))
                return true;
            else
                next_step = 2;
        case 2:
            x = { ++var_counter, "x" };
            parent_1 = new Parent(x, des);
            ancestor_1 = new Ancestor(ans, x);
            next_step = 3;
        case 3:
            while (parent_1->step(env, var_counter))
            {
                case 4:;
                if (ancestor_1->step(env, var_counter))
                {
                    next_step = 4;
                    return true;
                }
                ancestor_1->reset();
            }
            next_step = 5;
            return false;
        default:
            next_step = -1;
            return false;
    }
}
