#pragma once

#include "Environment.hpp"

#include "DataTypes.hpp"

#include <variant>
#include <string>
#include <iostream>
#include <cassert>
#include <vector>

class Parent
{
private:
    int next_step = 0;
    std::size_t checkpoint = 0;

    lvar<Person> p;
    lvar<Person> c;

    struct Result
    {
        Person p;
        Person c;
    } cached_result;

    std::ostream& print(std::ostream& os, int nesting);

    friend class Ancestor;

    bool bind(Environment& env);

public:
    Parent(lvar<Person> p, lvar<Person> c);
    void reset();

    bool step(Environment& env, int& var_counter);
};

/*
 Implements relation:
    parent(ans, des) ||
        parent(x, des) && ancestor(ans, x)
 */
class Ancestor
{
private:
    int next_step = 0;

    lvar<Person> ans;
    lvar<Person> des;
    lvar<Person> x = {};

    Parent* parent_0 = nullptr;

    Parent* parent_1 = nullptr;
    Ancestor* ancestor_1 = nullptr;

    // (The _0 and _1 above group terms into two disjunctions.)

    Person const* get_ans() const
    {
        if (parent_0)
            return &parent_0->cached_result.p;
        else if (ancestor_1)
            return ancestor_1->get_ans();
        else
            return nullptr;
    }

    Person const* get_des() const
    {
        if (parent_0)
            return &parent_0->cached_result.c;
        else if (parent_1)
            return &parent_1->cached_result.c;
        else
            return nullptr;
    }

    std::ostream& print_head(std::ostream& os) const
    {
        auto* arg0 = get_ans();
        auto* arg1 = get_des();

        os << "Ancestor(";

        if (get_ans())
            os << *get_ans();
        else
            os << "???";

        os << ",";

        if (get_des())
            os << *get_des();
        else
            os << "???";

        os << ")";

        return os;
    }

public:
    Ancestor(lvar<Person> ans, lvar<Person> des);
    ~Ancestor();

    void reset();

    bool step(Environment& env, int& var_counter);

    std::ostream& print(std::ostream& os, int nesting)
    {
        print_tabs(os, nesting);
        print_head(os) << " = {\n";

        if (parent_0)
            parent_0->print(os, nesting+1) << "\n";
        else if (parent_1 && ancestor_1)
        {
            parent_1->print(os, nesting+1) << " &\n";
            ancestor_1->print(os, nesting + 1) << "\n";
        }
        else
        {
            print_tabs(os, nesting+1)
               << "???\n";
        }

        print_tabs(os, nesting) << "}";

        return os;
    }
};
