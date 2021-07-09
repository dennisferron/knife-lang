#pragma once

#include "../Json.hpp"
#include "ProgramNamespace.hpp"

#include <fstream>
#include <string>

class Output
{
protected:
    std::ofstream ofs;
    JsonDocument document;

    Output(std::string file_name);

    void demo(JsonObject root);
    void write(JsonArray& arr, outp::RelationClass const& relation);
    void write_all_relations(
        JsonObject& json,
        std::vector<outp::RelationClass> const& relations);

    void write(JsonArray& arr, outp::StepCase const& step);
    void write_all_steps(
        JsonObject& json,
        std::vector<outp::StepCase> const& steps);

//    void write_all_statements(
//            JsonObject& json,
//            std::vector<lang::Statement> const& statements);
//
//    void write_statement(JsonArray& arr, lang::LetStatement const& statement);
//    void write_statement(JsonArray& arr, lang::FreshStatement const& statement);
//    void write_statement(JsonArray& arr, lang::YieldStatement const& statement);

public:
    ~Output();
    void close();
};

class OutputHeader : private Output
{
private:
    std::string base_name;

public:
    OutputHeader(std::string base_name);
    void write(outp::ProgramNamespace const& program);
};

class OutputSource : private Output
{
private:
    std::string base_name;

public:
    OutputSource(std::string base_name);
    void write(outp::ProgramNamespace const& program);
};

class OutputDbInit : private  Output
{
private:
    std::string base_name;

public:
    OutputDbInit(std::string base_name);
    void write(outp::ProgramNamespace const& program);
};
