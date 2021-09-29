#pragma once

#include "ParseLogger.hpp"

namespace knife::data
{
    class LogDatabase : public ParseLogger
    {
    private:
        sqlite3* db = nullptr;

        sqlite3_stmt* stmt_insert_token = nullptr;
        sqlite3_stmt* stmt_insert_rule_names = nullptr;
        sqlite3_stmt* stmt_insert_token_names = nullptr;
        sqlite3_stmt* stmt_insert_parse_context = nullptr;
        sqlite3_stmt* stmt_insert_expression = nullptr;
        sqlite3_stmt* stmt_update_expression_parent = nullptr;
        sqlite3_stmt* stmt_insert_expr_stack = nullptr;

        void exec(std::string sql);
        sqlite3_stmt* prepare(char const* sql);

        void create_tables();
        void prepare_statements();

    public:
        LogDatabase(std::string db_file);
        ~LogDatabase();

        void begin_transaction() override;
        void commit_transaction() override;
        void rollback_transaction() override;

        void insert_token_name(std::size_t tokenType, std::string tokenName) override;
        void insert_rule_name(std::size_t rule_index, std::string rule_name) override;
        void insert_token(antlr4::Token* token) override;
        void insert_parse_context(antlr4::ParserRuleContext* ctx) override;
        void insert_expression(lang::Expression expr, antlr4::ParserRuleContext const* ctx) override;
        void update_expression_parent(lang::Expression expr, lang::Expression parent) override;
    };
}
