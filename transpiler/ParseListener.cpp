#include "ParseListener.hpp"

ParseListener::ParseListener(ExprParser* parser)
    : parser(parser)
{
}

void ParseListener::enterSql_stmt_list(ExprParser::Sql_stmt_listContext *ctx)
{
    auto tokens = parser->getTokenStream();
    std::string text = tokens->getText(ctx);
    std::cout << "SQL statements: "
        << text << "\n";

    //System.out.println("\t"+type+" "+ctx.Identifier()+args+";");
}
