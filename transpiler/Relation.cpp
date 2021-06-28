#include "Relation.hpp"

namespace lang
{

    Relation::Relation(std::string name) :
            name(name)
    {}

    std::string Relation::get_name() const
    {
        return name;
    }

    void Relation::add_statement(const Statement& stmt)
    {
        statements.push_back(stmt);
    }

    const std::vector<Statement>& Relation::get_statements() const
    {
        return statements;
    }

}