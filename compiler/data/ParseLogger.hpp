#pragma once

#include "antlr4-runtime.h"
#include "sqlite3.h"
#include "../lang/Expression.hpp"

#include <string>
#include <memory>

namespace knife::data
{
    class ParseLogger
    {
    public:
        virtual void begin_transaction() = 0;
        virtual void commit_transaction() = 0;
        virtual void rollback_transaction() = 0;

        virtual void insert_token_name(std::size_t tokenType, std::string tokenName) = 0;
        virtual void insert_rule_name(std::size_t rule_index, std::string rule_name) = 0;
        virtual void insert_token(antlr4::Token* token) = 0;
        virtual void insert_parse_context(antlr4::ParserRuleContext* ctx) = 0;
        virtual void insert_expression(lang::Expression expr, antlr4::ParserRuleContext const* ctx) = 0;
        virtual void update_expression_parent(lang::Expression expr, lang::Expression parent) = 0;
    };

}
