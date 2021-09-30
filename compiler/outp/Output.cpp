#include "Output.hpp"

#include "../Json.hpp"

using namespace knife;
using namespace knife::outp;

Output::Output(std::ostream& ofs)
    : document(ofs)
{
}

Output::~Output()
{
    close();
}

void Output::close()
{
    document.close();
}

void Output::demo(JsonObject root)
{
    root["vars"] = [](JsonArray arr)
    {
        arr("p", "c");
    };

    root["some_obj"] = [](JsonObject obj)
    {
        obj["x"] = 1;
    };

    root["statements"] = [](JsonArray a)
    {
        a += [](JsonObject o)
        {
            o["template"] = "assign_statement";
            o["args"] = [](JsonObject o)
            {
                o["assignee"] = "x";
                o["expr"] = "2+b";
            };
        };

        a += [](JsonObject o)
        {
            o["template"] = "print_statement";
            o["args"] = [](JsonObject o)
            {
                o["var"] = "x";
            };
        };

        a += [](JsonObject o)
        {
            o["template"] = "return_statement";
            o["args"] = [](JsonObject o)
            {
                o["value"] = "42";
            };
        };
    };
}

//void Output::write_statement(JsonArray& arr, lang::LetStatement const& statement)
//{
//    arr += [&](JsonObject o)
//    {
//        o["type"] = "let statement";
//        o["template"] = "return_statement";
//        o["args"] = [](JsonObject o)
//        {
//            o["value"] = "42";
//        };
//    };
//}
//
//void Output::write_statement(JsonArray& arr, lang::FreshStatement const& statement)
//{
//    arr += [&](JsonObject o)
//    {
//        o["type"] = "fresh statement";
//        o["template"] = "return_statement";
//        o["args"] = [](JsonObject o)
//        {
//            o["value"] = "42";
//        };
//    };
//}
//
//void Output::write_statement(JsonArray& arr, lang::YieldStatement const& statement)
//{
//    arr += [&](JsonObject o)
//    {
//        o["type"] = "yield statement";
//        o["template"] = "return_statement";
//        o["args"] = [](JsonObject o)
//        {
//            o["value"] = "42";
//        };
//    };
//}
//
//void Output::write_all_statements(
//        JsonObject& json,
//        std::vector<lang::Statement> const& statements)
//{
//    json["statements"] = [&](JsonArray arr)
//    {
//        for (auto const& stmt : statements)
//        {
//            auto action = [&](auto s) { write_statement(arr, s); };
//            std::visit(action, stmt);
//        }
//    };
//}

void Output::write(JsonArray& arr, outp::RelationClass const& relation)
{
    arr += [&](JsonObject o)
    {
        o["name"] = relation.name;
        o["parm_lvars"] = [&](JsonArray arr)
        {
            for (auto v : relation.param_lvars)
            {
                arr += [&](JsonObject o)
                {
                    o["name"] = v.var_name;
                    o["type"] = v.type;
                    o["comment"] = v.comment;
                };
            }
        };
        o["rslt_vars"] = [&](JsonArray arr)
        {
            for (auto v : relation.result_vars)
            {
                arr += [&](JsonObject o)
                {
                    o["name"] = v.var_name;
                    o["type"] = v.type;
                    o["comment"] = v.comment;
                };
            }
        };
        o["fresh_lvars"] = [&](JsonArray arr)
        {
            for (auto v : relation.fresh_lvars)
            {
                arr += [&](JsonObject o)
                {
                    o["name"] = v.var_name;
                    o["type"] = v.type;
                    o["comment"] = v.comment;
                };
            }
        };
        o["mbrs"] = [&](JsonArray arr)
        {
            for (auto mbr : relation.members)
            {
                arr += [&](JsonObject o)
                {
                    o["name"] = mbr.rel_name;
                    o["type"] = mbr.rel_type;
                };
            }
        };

        write_all_steps(o, relation.step_cases);
        //write_all_statements(o, relation.get_statements());
        o["statements"] = [&](JsonArray arr){};
    };
}

void Output::write_all_relations(
        JsonObject& json,
        std::vector<outp::RelationClass> const& relations)
{
    json["relations"] = [&](JsonArray arr)
    {
        for (auto const& r : relations)
            write(arr, r);
    };
}

void Output::write_all_steps(
        JsonObject& json,
        std::vector<outp::StepCase> const& steps)
{
    json["step_cases"] = [&](JsonArray arr)
    {
        for (auto const& s : steps)
            write(arr, s);
    };
}

void Output::write(JsonArray& arr, outp::StepCase const& step)
{
    arr += [&](JsonObject o)
    {
        o["number"] = step.number;
        o["next_step"] = step.number+1;

        auto action = [&](auto const& s) mutable
            { write_case_template(o, s); };
        std::visit(action, step.case_template);
    };
}

void Output::write_case_template(JsonObject& o, const outp::Case_SetResult& c)
{
    o["template"] = "case_set_result";
    o["result"] = [&](JsonArray a)
    {
        // The initializer list for Result { ... };
        for (auto const& data : c.result)
        {
            a += data;
        }
    };
}

void Output::write(JsonArray& arr, const outp::SubRel& s)
{
    arr += [&](JsonObject o)
    {
        o["rel_name"] = s.rel_name;
        o["rel_type"] = s.rel_type;
        o["rel_args"] = [&](JsonArray arr)
        {
            for (auto const& arg : s.rel_args)
                arr += arg;
        };
    };
}

void Output::write(JsonArray& arr, const outp::FreshVar& v)
{
    arr += [&](JsonObject o)
    {
        o["var_name"] = v.var_name;
        o["type"] = v.type;
        o["comment"] = v.comment;
    };
}

void Output::write_case_template(JsonObject& o, const outp::Case_InitSubRel& c)
{
    o["template"] = "case_init_sub_rel";
    o["prev_rels"] = [&](JsonArray arr)
    {
        for (auto const& r : c.prev_rels)
            write(arr, r);
    };

    o["fresh_vars"] = [&](JsonArray arr)
    {
        for (auto const& v : c.fresh_vars)
            write(arr, v);
    };

    o["init_rels"] = [&](JsonArray arr)
    {
        for (auto const& r : c.init_rels)
            write(arr, r);
    };
}

OutputHeader::OutputHeader(std::ostream& ofs, std::string base_name)
    : Output(ofs), base_name(base_name)
{
}

void OutputHeader::write(outp::ProgramNamespace const& program)
{
    JsonObject& root = document.get_root();
    root["projectNamespace"] = base_name;
    write_all_relations(root, program.classes);
    demo(root);
}

OutputSource::OutputSource(std::ostream& ofs, std::string base_name)
        : Output(ofs), base_name(base_name)
{
}

void OutputSource::write(outp::ProgramNamespace const& program)
{
    JsonObject& root = document.get_root();
    root["projectNamespace"] = base_name;
    write_all_relations(root, program.classes);
    demo(root);
}

OutputDbInit::OutputDbInit(std::ostream& ofs, std::string base_name)
        : Output(ofs), base_name(base_name)
{
}

void OutputDbInit::write(outp::ProgramNamespace const& program)
{
    JsonObject& root = document.get_root();
    demo(root);
}
