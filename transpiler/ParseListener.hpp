#pragma once

#include "KnifeParserBaseListener.h"
#include "lang/Program.hpp"
#include "lang/Expression.hpp"
#include "data/LogDatabase.hpp"

#include <stack>
#include <map>

class ParseListener : public KnifeParserBaseListener
{
private:
    KnifeParser* parser;
    lang::Program* program;
    data::LogDatabase* log_database;

    std::map<
        antlr4::ParserRuleContext const*,
        lang::Expression const*
    > expr_map;

    lang::Expression const* get_expr(antlr4::ParserRuleContext* ctx);
    void put_expr(lang::Expression const* expr,
                  antlr4::ParserRuleContext* ctx);
    void put_binop(std::string op, antlr4::ParserRuleContext* ctx,
                   antlr4::ParserRuleContext* lhs,
                   antlr4::ParserRuleContext* rhs);
    void print_exprs(std::ostream& os);

public:
    ParseListener(KnifeParser* parser, lang::Program* program, data::LogDatabase* log_database);

    virtual void enterEveryRule(antlr4::ParserRuleContext* ctx) override;
    virtual void exitStat(KnifeParser::StatContext * /*ctx*/) override;

    //void enterSql_stmt_list(KnifeParser::Sql_stmt_listContext *ctx) override;

    void enterQuasiQuoteSql(KnifeParser::QuasiQuoteSqlContext* ctx) override;
    void enterQuasiQuoteCsv(KnifeParser::QuasiQuoteCsvContext* ctx) override;

    void enterRelation_name(KnifeParser::Relation_nameContext* ctx) override;
    void enterRelation_param(KnifeParser::Relation_paramContext* ctx) override;

    virtual void enterLet_stmt(KnifeParser::Let_stmtContext* ctx) override;
    virtual void enterFresh_stmt(KnifeParser::Fresh_stmtContext* ctx) override;
    virtual void enterYield_stmt(KnifeParser::Yield_stmtContext* ctx) override;

    virtual void exitMember_stmt(KnifeParser::Member_stmtContext* ctx) override;

    virtual void exitDotExpr(KnifeParser::DotExprContext* ctx) override;
    virtual void exitIntExpr(KnifeParser::IntExprContext* ctx) override;
    virtual void exitQuasiquoteExpr(KnifeParser::QuasiquoteExprContext* ctx) override;
    virtual void exitBinOpExpr2(KnifeParser::BinOpExpr2Context* ctx) override;
    virtual void exitBinOpExpr1(KnifeParser::BinOpExpr1Context* ctx) override;
    virtual void exitCallExpr(KnifeParser::CallExprContext* ctx) override;
    virtual void exitParenExpr(KnifeParser::ParenExprContext* ctx) override;
    virtual void exitIdExpr(KnifeParser::IdExprContext* ctx) override;
    virtual void exitIdentifier(KnifeParser::IdentifierContext * ctx) override;
};


