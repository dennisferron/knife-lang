#pragma once

#include "ParseLogger.hpp"

namespace knife::data
{

    class NullLogger : public ParseLogger
    {
    public:
        void begin_transaction() override
        {

        }

        void commit_transaction() override
        {

        }

        void rollback_transaction() override
        {

        }

        void insert_token_name(std::size_t tokenType, std::string tokenName) override
        {

        }

        void insert_rule_name(std::size_t rule_index, std::string rule_name) override
        {

        }

        void insert_token(antlr4::Token* token) override
        {

        }

        void insert_parse_context(antlr4::ParserRuleContext* ctx) override
        {

        }

        void insert_expression(const lang::Expression* expr, const antlr4::ParserRuleContext* ctx) override
        {

        }

        void update_expression_parent(const lang::Expression* expr, const lang::Expression* parent) override
        {

        }

    };

}
