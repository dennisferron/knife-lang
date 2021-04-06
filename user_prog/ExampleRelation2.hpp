#pragma once

#include "Environment.hpp"

#include <variant>
#include <string>
#include <iostream>
#include <cassert>
#include <vector>

struct Person
{
    std::string name;

    bool operator ==(Person const& y) const
    {
        return this->name == y.name;
    }

    friend std::ostream& operator <<(std::ostream& os, Person const& x)
    {
        return os << x.name;
    }
};

class Parent : public Binding
{
private:
    int next_step = 0;
    lvar<Person> p;
    lvar<Person> c;

    struct Result
    {
        Person p;
        Person c;
    } cached_result;

    friend std::ostream& operator <<(std::ostream& os, Parent const& x)
    {
        return os << "Parent("
            << x.cached_result.p << ","
            << x.cached_result.c << ")";
    }

    friend class Ancestor;

    bool bind(Environment& env);

public:
    Parent(lvar<Person> p, lvar<Person> c);
    void reset() { next_step = 0; }

    bool step(Environment& env, int& var_counter);

    void const* lookup(int v) const override;
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

    std::ostream& write_head(std::ostream& os) const
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

    std::ostream& write_tabs(std::ostream& os, int tabs)
    {
        for (int i=0; i<tabs; i++)
            os << "   ";

        return os;
    }

public:
    Ancestor(lvar<Person> ans, lvar<Person> des);
    ~Ancestor();

    void reset();

    bool step(Environment& env, int& var_counter);

    std::ostream& write(std::ostream& os, int nesting)
    {
        write_tabs(os, nesting);
        write_head(os);
        os << "\n";
        write_tabs(os, nesting);
        os << "{\n";
        write_tabs(os, nesting+1);

        if (parent_0)
            os << *parent_0;
        else if (parent_1 && ancestor_1)
        {
            os << *parent_1 << ";\n";
            ancestor_1->write(os, nesting + 1);
        }
        else
            os << "???";

        os << "\n";
        write_tabs(os, nesting);
        os << "}";

        return os;
    }
};
