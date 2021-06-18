#pragma once

#include <string>

namespace lang
{

    class Relation
    {
    private:
        std::string name;

    public:
        explicit Relation(std::string name);
        std::string get_name() const;
    };

}
