#include "Relation.hpp"

lang::Relation::Relation(std::string name) : name(name) {}

std::string lang::Relation::get_name() const
{
    return name;
}
