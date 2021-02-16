#include "ParseListener.hpp"

ParseListener::ParseListener(ExprParser* parser)
    : parser(parser)
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
    std::cout << "SQL statements: "
              << text << "\n";
}

void ParseListener::enterQuasiQuoteCsv(ExprParser::QuasiQuoteCsvContext* ctx)
{
    auto tokens = parser->getTokenStream();
    std::string text = tokens->getText(ctx->csv_row_list());
    std::cout << "CSV rows: "
              << text << "\n";
}

