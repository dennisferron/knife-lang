#include "Program.hpp"

#include <stdexcept>

lang::Relation& lang::Program::last_relation()
{
    if (relations.empty())
        throw std::logic_error("No relations defined.");
    else
    {
        return *relations.rbegin();
    }
}
