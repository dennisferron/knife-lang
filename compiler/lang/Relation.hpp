#pragma once

#include "Statement.hpp"

#include <string>
#include <vector>

namespace knife::lang
{
    struct ParamVar
    {
        std::string name;
        std::string type;
    };

    struct ResultVar
    {
        std::string name;
        std::string type;
    };

    class Relation
    {
    private:
        std::string name;
        std::vector<ParamVar> params;
        std::vector<Statement> statements;

    public:
        explicit Relation(std::string name);
        std::string get_name() const;
        void add_statement(Statement const& stmt);
        void add_param(ParamVar const& param);
        std::vector<Statement> const& get_statements() const;
        std::vector<ParamVar> const& get_params() const { return params; }

        // TODO:  Actually calculate cached result as needed from statements.
        std::vector<ResultVar> get_result_vars() const
        {
            std::vector<ResultVar> result;
            for (auto const& v : params)
                result.push_back({v.name, v.type});
            return result;
        }
    };

}
