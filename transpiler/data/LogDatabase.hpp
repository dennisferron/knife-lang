#pragma once

#include "antlr4-runtime.h"
#include "sqlite3.h"

#include <string>
#include <memory>

namespace data
{
    class LogDatabase
    {
    private:
        sqlite3* db = nullptr;

        void exec(std::string sql);
        void create_tables();

        friend class TokenInserter;

    public:
        LogDatabase(std::string db_file);
        ~LogDatabase();

        void begin_transaction();
        void commit_transaction();
        void rollback_transaction();
    };

    class TokenInserter
    {
    private:
        sqlite3_stmt* stmt = nullptr;

    public:
        TokenInserter(LogDatabase const& db);
        ~TokenInserter();
        void insert(antlr4::Token* token);
    };
}
