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
        friend class TokenNamesInserter;
        friend class ParserRuleContextInserter;

    public:
        LogDatabase(std::string db_file);
        ~LogDatabase();

        void begin_transaction();
        void commit_transaction();
        void rollback_transaction();
    };

    class TokenNamesInserter
    {
    private:
        sqlite3_stmt* stmt = nullptr;

    public:
        TokenNamesInserter(LogDatabase const& db);
        ~TokenNamesInserter();
        void insert(std::size_t tokenType, std::string tokenName);
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

    class ParserRuleContextInserter
    {
    private:
        sqlite3_stmt* stmt = nullptr;

    public:
        ParserRuleContextInserter(LogDatabase const& db);
        ~ParserRuleContextInserter();
        void insert(antlr4::ParserRuleContext* ctx);
    };
}
