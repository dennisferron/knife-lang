#pragma once

// Logic var was pointer to pointer only because
// the design was somehow relying on the fact all
// pointers are the same size to make something work.
// And/or because the intermediate pointers in the relation
// expression objects meant they could be passed by value.
// The actual values were stored in a library-managed heap.

// Another reason to use a T** for lvars is that then we
// can unify two variables with each other without needing
// a ground value in order to make that work.

// Instead of making every expression component fully general
// which requires defensive coding and instead of distributing
// knowledge, the compiler just needs to keep track of things
// like which variables are ground after an expression and
// whether two variables ever bind with each other or only
// with values.  Then the compiler can just emit the level
// of complexity which is required to support the features.

template <typename T, typename U>
class EqVarVal
{
private:
    bool has_stepped = false;
    T const** const logic_var;
    U const cached_val;

public:
    EqVarVal(T const** const& init_var, U const& init_val)
        : logic_var(init_var), cached_val(init_val) {}


};

class Letters
{
private:
    int next_case = 0;
    char value;
    char* variable;

public:
    bool step();
};

class Numbers
{
private:
    int next_case = 0;

public:
    bool step();
    void reset() { next_case = 0; }
};

class ExampleRelation
{
private:
    int next_case = 0;
    Letters rel_0;
    Numbers rel_1;

public:
    bool step()
    {
        switch (next_case)
        {
            case 0:
                while (rel_0.step())
                {
                    case 1:;
                    if (rel_1.step())
                    {
                        next_case=1;
                        return true;
                    }
                    rel_1.reset();
                }
            default:
                return false;
        }
    }
};


