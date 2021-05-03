#include "ExampleRelation2.hpp"

std::ostream& Parent::print(std::ostream& os, int nesting)
{
    return print_tabs(os, nesting) << "Parent("
              << cached_result.p << ","
              << cached_result.c << ")";
}

Parent::Parent(lvar<Person> p, lvar<Person> c)
    : p(p), c(c)
{
}

void Parent::reset()
{
    next_step = 0;
}

bool Parent::bind(Environment& env)
{
    checkpoint = env.save_checkpoint();

    bool success = env.eq(p, &cached_result.p)
            && env.eq(c, &cached_result.c);

    if (!success)
        env.revert_to_checkpoint(checkpoint);

    return success;
}

bool Parent::step(Environment& env, int& var_counter)
{
    // Must remove previous bindings before we can bind new.
    if (next_step > 0)
        env.revert_to_checkpoint(checkpoint);

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
            env.revert_to_checkpoint(checkpoint);
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
            delete parent_0;
            parent_0 = nullptr;
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
