#pragma once

#include <ostream>
#include <optional>

// The unit test library needs to print values, but
// the standard library doesn't define operator <<
// for optional<T>. Putting user code in namespace std
// isn't encouraged, but there are no good solutions to
// this problem.  This is the simplest bad solution.
namespace std
{
    inline ostream& operator <<(ostream& os, nullopt_t const&)
    {
        return os << "empty";
    }

    template <typename T>
    ostream& operator <<(ostream& os, optional<T> value)
    {
        if (value)
            return os << value;
        else
            return os << "empty";
    }
}
