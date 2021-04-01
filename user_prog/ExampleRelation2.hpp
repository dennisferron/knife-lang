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
};

class Parent
{
private:
    int next_step = 0;
    lvar<Person> p;
    lvar<Person> c;

public:
    Parent(lvar<Person> p, lvar<Person> c);
    void reset() { next_step = 0; }

    bool step(Environment const& env, int& var_counter);
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

public:
    Ancestor(lvar<Person> ans, lvar<Person> des);
    ~Ancestor();

    void reset();

    bool step(Environment const& env, int& var_counter);
};
