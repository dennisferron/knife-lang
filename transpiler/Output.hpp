#pragma once

#include "Json.hpp"
#include "Program.hpp"

#include <fstream>
#include <string>

class Output
{
protected:
    std::ofstream ofs;
    JsonDocument document;

    Output(std::string file_name);

    void demo(JsonObject root);
    void write(JsonArray& arr, lang::Relation const& relation);
    void write_all_relations(
        JsonObject& json,
        std::vector<lang::Relation> const& relations);

    void write_all_statements(
            JsonObject& json,
            std::vector<lang::Statement> const& statements);

    void write_statement(JsonArray& arr, lang::LetStatement const& statement);
    void write_statement(JsonArray& arr, lang::FreshStatement const& statement);
    void write_statement(JsonArray& arr, lang::YieldStatement const& statement);

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
    void write(lang::Program const& program);
};

class OutputSource : private Output
{
private:
    std::string base_name;

public:
    OutputSource(std::string base_name);
    void write(lang::Program const& program);
};

class OutputDbInit : private  Output
{
private:
    std::string base_name;

public:
    OutputDbInit(std::string base_name);
    void write(lang::Program const& program);
};
