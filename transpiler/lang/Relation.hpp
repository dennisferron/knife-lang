#pragma once

#include "Statement.hpp"

#include <string>
#include <vector>

namespace lang
{

    class Relation
    {
    private:
        std::string name;
        std::vector<Statement> statements;

    public:
        explicit Relation(std::string name);
        std::string get_name() const;
        void add_statement(Statement const& stmt);
        std::vector<Statement> const& get_statements() const;
    };

}
