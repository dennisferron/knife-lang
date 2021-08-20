#include "ParseListener.hpp"

ParseListener::ParseListener(ExprParser* parser, lang::Program* program)
    : parser(parser), program(program)
{
}

void ParseListener::enterSql_stmt_list(ExprParser::Sql_stmt_listContext *ctx)
{
//    auto tokens = parser->getTokenStream();
//    std::string text = tokens->getText(ctx);
//    std::cout << "SQL statements: "
//        << text << "\n";

    //System.out.println("\t"+type+" "+ctx.Identifier()+args+";");
}

void ParseListener::enterQuasiQuoteSql(ExprParser::QuasiQuoteSqlContext* ctx)
{
    auto tokens = parser->getTokenStream();
    std::string text = tokens->getText(ctx->sql_stmt_list());
    //std::cout << "SQL statements: "
    //          << text << "\n";
}

void ParseListener::enterQuasiQuoteCsv(ExprParser::QuasiQuoteCsvContext* ctx)
{
    auto tokens = parser->getTokenStream();
    std::string text = tokens->getText(ctx->csv_row_list());
    std::cout << "CSV rows: "
              << text << "\n";
}

void ParseListener::enterRelation_name(ExprParser::Relation_nameContext* ctx)
{
    std::string id = ctx->EXPR_ID()->getText();
    program->relations.push_back(lang::Relation(id));
}

void ParseListener::enterRelation_param(ExprParser::Relation_paramContext* ctx)
{
    std::string name = ctx->EXPR_ID(0)->getText();
    std::string type = "";
    if (ctx->EXPR_ID(1))
        type = ctx->EXPR_ID(1)->getText();
    program->last_relation().add_param(lang::ParamVar {name, type});
}

void ParseListener::enterLet_stmt(ExprParser::Let_stmtContext* ctx)
{
    program->last_relation().add_statement(lang::LetStatement());
}

void ParseListener::enterFresh_stmt(ExprParser::Fresh_stmtContext* ctx)
{
    program->last_relation().add_statement(lang::FreshStatement());
}

void ParseListener::enterYield_stmt(ExprParser::Yield_stmtContext* ctx)
{
    program->last_relation().add_statement(lang::YieldStatement());
}

void ParseListener::enterMember_stmt(ExprParser::Member_stmtContext* ctx)
{
    lang::MemberStatement stmt;
    stmt.name = ctx->member_name->getText();
    stmt.type = ctx->member_type->getText();
    for (auto a : ctx->call_param_list()->children)
        if (a->getText() != ",") // TODO: exclude comma more elegantly
            stmt.args.push_back(a->getText());
    program->last_relation().add_statement(stmt);
}
