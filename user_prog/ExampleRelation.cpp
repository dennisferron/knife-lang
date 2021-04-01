//
// Created by dferr on 2/27/2021.
//

#include "ExampleRelation.hpp"

bool Letters::step()
{
    switch (next_case)
    {
        case 0:
            // bind 'A' to a variable
            next_case = 1;
            return true;
        case 1:
            // bind 'B' to a variable
            next_case = 2;
            return true;
        case 2:
            // bind 'C' to a variable
            next_case = 3;
            return true;
        default:
            return false;
    }
}

bool Numbers::step()
{
    switch (next_case)
    {
        case 0:
            // bind '1' to a variable
            next_case = 1;
            return true;
        case 1:
            // bind '2' to a variable
            next_case = 2;
            return true;
        default:
            return false;
    }
}
