#pragma once

#include "ExprParserBaseListener.h"
#include "Program.hpp"

class ParseListener : public ExprParserBaseListener
{
private:
    ExprParser* parser;
    lang::Program* program;

public:
    ParseListener(ExprParser* parser, lang::Program* program);

    void enterSql_stmt_list(ExprParser::Sql_stmt_listContext *ctx) override;

    void enterQuasiQuoteSql(ExprParser::QuasiQuoteSqlContext* ctx) override;
    void enterQuasiQuoteCsv(ExprParser::QuasiQuoteCsvContext* ctx) override;

    void enterRelation_name(ExprParser::Relation_nameContext* ctx) override;
};


