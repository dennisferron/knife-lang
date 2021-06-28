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

    virtual void enterLet_stmt(ExprParser::Let_stmtContext* ctx) override;
    virtual void enterFresh_stmt(ExprParser::Fresh_stmtContext* ctx) override;
    virtual void enterYield_stmt(ExprParser::Yield_stmtContext* ctx) override;
};


