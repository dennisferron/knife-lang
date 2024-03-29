#include "Transform.hpp"

namespace knife
{

outp::ProgramNamespace transform(const lang::Program& inp_prog)
{
    outp::ProgramNamespace result;

    for (auto const& rel : inp_prog.relations)
        result.classes.push_back(transform(rel));

    return result;
}

struct TransformStatement
{
    std::vector<lang::MemberStatement> members;
    std::vector<lang::FreshStatement> fresh_vars;

    void operator()(...) {}
    void operator()(lang::MemberStatement const& stmt)
    {
        members.push_back(stmt);
    }
    void operator()(lang::FreshStatement const& stmt)
    {
        fresh_vars.push_back(stmt);
    }
    void operator()(lang::YieldStatement const& stmt)
    {
    }
};

outp::RelationClass transform(const lang::Relation& inp_rel)
{
    outp::RelationClass outp_rel;

    outp_rel.name = inp_rel.get_name();

    TransformStatement tr_stmt;
    for (auto stmt : inp_rel.get_statements())
        std::visit(tr_stmt, stmt);

    for (auto mbr : tr_stmt.members)
    {
        // TODO:  this could be a "member" struct
        // instead of a "subrel" struct.
        outp::SubRel sr;
        sr.rel_name = mbr.name;
        sr.rel_type = mbr.type;
        sr.rel_args = mbr.args;
        outp_rel.members.push_back(sr);
    }

    for (auto const& p : inp_rel.get_params())
    {
        outp_rel.param_lvars.push_back(outp::ParamVar {p.name, p.type, "from param_lvars"});
    }

    for (auto frsh_stmt : tr_stmt.fresh_vars)
    {
        outp::FreshVar frsh_var =
        {
            frsh_stmt.name,
            frsh_stmt.type,
            "from fresh var"
        };
        outp_rel.fresh_lvars.push_back(frsh_var);
    }

    for (auto const& p : inp_rel.get_result_vars())
    {
        outp_rel.result_vars.push_back(outp::ResultVar {p.name, p.type, "from result_vars"});
    }

    // TODO: convert tr_stmt into StepCases

    outp::StepCase case0;
    case0.number = 0;
    case0.case_template = outp::Case_InitSubRel
    {
            outp_rel.members, // prev_rels,
        {}, //{ outp::FreshVar { "x" } }, // fresh_vars
        outp_rel.members  // init_rels
    };
    outp_rel.step_cases.push_back(case0);

    outp::StepCase case1;
    case1.number = 1;
    case1.case_template = outp::Case_SetResult
    {
        { "alice", "bob" }
    };
    outp_rel.step_cases.push_back(case1);

    return outp_rel;
}

}