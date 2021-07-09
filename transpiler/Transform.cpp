#include "Transform.hpp"

outp::ProgramNamespace transform(const lang::Program& inp_prog)
{
    outp::ProgramNamespace result;

    for (auto const& rel : inp_prog.relations)
        result.classes.push_back(transform(rel));

    return result;
}

outp::RelationClass transform(const lang::Relation& inp_rel)
{
    outp::RelationClass result;

    result.name = inp_rel.get_name();

    outp::StepCase case0;
    case0.number = 0;
    case0.result = {"alice", "bob"};
    result.step_cases.push_back(case0);

    outp::StepCase case1;
    case1.number = 1;
    case1.result = {"bob", "chris"};
    result.step_cases.push_back(case1);

    return result;
}
