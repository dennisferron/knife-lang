#pragma once

#include "antlr4-runtime.h"
#include "sqlite3.h"
#include "../lang/Expression.hpp"

#include <string>
#include <memory>

namespace data
{
    class LogDatabase
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

        void begin_transaction();
        void commit_transaction();
        void rollback_transaction();

        void insert_token_name(std::size_t tokenType, std::string tokenName);
        void insert_rule_name(std::size_t rule_index, std::string rule_name);
        void insert_token(antlr4::Token* token);
        void insert_parse_context(antlr4::ParserRuleContext* ctx);
        void insert_expression(lang::Expression const* expr, antlr4::ParserRuleContext const* ctx);
        void update_expression_parent(lang::Expression const* expr, lang::Expression const* parent);
        void insert_stack(lang::Expression const* pushExpr, std::size_t numPopped, int generation);
    };
}
