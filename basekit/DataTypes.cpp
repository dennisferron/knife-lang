#include "DataTypes.hpp"

std::ostream& print_tabs(std::ostream& os, int tabs)
{
    for (int i=0; i<tabs; i++)
        os << "   ";

    return os;
}
