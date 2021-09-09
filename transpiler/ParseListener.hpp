#pragma once

#include "ExprParserBaseListener.h"
#include "lang/Program.hpp"
#include "lang/Expression.hpp"
#include "data/LogDatabase.hpp"

#include <stack>
#include <map>

class ParseListener : public ExprParserBaseListener
{
private:
    ExprParser* parser;
    lang::Program* program;
    data::LogDatabase* log_database;

    std::map<
        antlr4::tree::ParseTree const*,
        lang::Expression const*
    > expr_map;

    lang::Expression const* get_expr(antlr4::tree::ParseTree* ctx);
    void put_expr(lang::Expression const* expr,
                  antlr4::ParserRuleContext* ctx);
    void put_binop(std::string op, antlr4::ParserRuleContext* ctx);
    void print_exprs(std::ostream& os);

public:
    ParseListener(ExprParser* parser, lang::Program* program, data::LogDatabase* log_database);

    virtual void enterEveryRule(antlr4::ParserRuleContext* ctx) override;
    virtual void exitStat(ExprParser::StatContext * /*ctx*/) override;

    void enterSql_stmt_list(ExprParser::Sql_stmt_listContext *ctx) override;

    void enterQuasiQuoteSql(ExprParser::QuasiQuoteSqlContext* ctx) override;
    void enterQuasiQuoteCsv(ExprParser::QuasiQuoteCsvContext* ctx) override;

    void enterRelation_name(ExprParser::Relation_nameContext* ctx) override;
    void enterRelation_param(ExprParser::Relation_paramContext* ctx) override;

    virtual void enterLet_stmt(ExprParser::Let_stmtContext* ctx) override;
    virtual void enterFresh_stmt(ExprParser::Fresh_stmtContext* ctx) override;
    virtual void enterYield_stmt(ExprParser::Yield_stmtContext* ctx) override;

    virtual void exitMember_stmt(ExprParser::Member_stmtContext* ctx) override;

    virtual void exitDotExpr(ExprParser::DotExprContext* ctx) override;
    virtual void exitIntExpr(ExprParser::IntExprContext* ctx) override;
    virtual void exitQuasiquoteExpr(ExprParser::QuasiquoteExprContext* ctx) override;
    virtual void exitBinOpExpr2(ExprParser::BinOpExpr2Context* ctx) override;
    virtual void exitBinOpExpr1(ExprParser::BinOpExpr1Context* ctx) override;
    virtual void exitCallExpr(ExprParser::CallExprContext* ctx) override;
    virtual void exitParenExpr(ExprParser::ParenExprContext* ctx) override;
    virtual void exitIdExpr(ExprParser::IdExprContext* ctx) override;
    virtual void exitExpr_identifier(ExprParser::Expr_identifierContext * ctx) override;
};


