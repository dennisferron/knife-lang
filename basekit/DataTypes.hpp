#pragma once

#include <string>
#include <ostream>

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

std::ostream& print_tabs(std::ostream& os, int tabs);


