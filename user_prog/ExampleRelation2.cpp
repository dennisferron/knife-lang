#include "ExampleRelation2.hpp"

Parent::Parent(lvar<Person> p, lvar<Person> c)
    : p(p), c(c)
{
}

bool Parent::step(Environment const& env, int& var_counter)
{
    switch (next_step)
    {
        case 0:
            next_step = 1;
            // Result { "alice", "bob" };
            return true;
        case 1:
            next_step = 2;
            // Result { "bob", "chris" };
            return true;
        case 2:
            next_step = 3;
            // Result { "chris", "debbie" };
            return true;
        case 3:
            next_step = 4;
            // Result { "debbie", "edith" };
            return true;
        default:
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

bool Ancestor::step(Environment const& env, int& var_counter)
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
