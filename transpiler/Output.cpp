#include "Output.hpp"

#include "Json.hpp"

Output::Output(std::string file_name)
    : ofs(file_name), document(ofs)
{
}

Output::~Output()
{
    close();
}

void Output::close()
{
    if (ofs.is_open())
    {
        document.close();
        ofs.close();
    }
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

void Output::write(JsonArray& arr, lang::Relation const& relation)
{
    arr += [&](JsonObject o)
    {
        o["name"] = relation.get_name();
        demo(o);
//        o["vars"] = [](JsonArray arr)
//        {
//            arr("p", "c");
//        };
    };
}

void Output::write_all_relations(
        JsonObject& json,
        std::vector<lang::Relation> const& relations)
{
    json["relations"] = [&](JsonArray arr)
    {
        for (auto const& r : relations)
            write(arr, r);
    };
}

OutputHeader::OutputHeader(std::string base_name)
    : Output(base_name + "_header.json"), base_name(base_name)
{
}

void OutputHeader::write(lang::Program const& program)
{
    JsonObject& root = document.get_root();
    root["projectNamespace"] = base_name;
    write_all_relations(root, program.relations);
    demo(root);
}

OutputSource::OutputSource(std::string base_name)
        : Output(base_name + "_source.json"), base_name(base_name)
{
}

void OutputSource::write(lang::Program const& program)
{
    JsonObject& root = document.get_root();
    root["projectNamespace"] = base_name;
    write_all_relations(root, program.relations);
    demo(root);
}

OutputDbInit::OutputDbInit(std::string base_name)
        : Output(base_name + "_db_init.json"), base_name(base_name)
{
}

void OutputDbInit::write(lang::Program const& program)
{
    JsonObject& root = document.get_root();
    demo(root);
}
