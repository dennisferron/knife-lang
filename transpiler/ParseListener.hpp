#pragma once

#include "ExprParserBaseListener.h"

class ParseListener : public ExprParserBaseListener
{
private:
    ExprParser* parser;

public:
    ParseListener(ExprParser* parser);
    void enterSql_stmt_list(ExprParser::Sql_stmt_listContext *ctx) override;
};


