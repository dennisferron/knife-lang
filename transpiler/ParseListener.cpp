#include "ParseListener.hpp"

ParseListener::ParseListener(ExprParser* parser, lang::Program* program, data::LogDatabase* log_database)
    : parser(parser), program(program), log_database(log_database)
{
}

void ParseListener::enterEveryRule(antlr4::ParserRuleContext* ctx)
{
    log_database->insert_parse_context(ctx);
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
    //std::string text = tokens->getText(ctx->sql_stmt_list());
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

void ParseListener::exitMember_stmt(ExprParser::Member_stmtContext* ctx)
{
    auto& os = std::cout;

    auto member_name = get_expr(ctx->member_name);
    auto member_type = get_expr(ctx->member_type);

    lang::MemberStatement stmt;
    stmt.name = ctx->member_name->getText();
    stmt.type = ctx->member_type->getText();
    for (auto a : ctx->call_param_list()->children)
    {
        if (a->getText() != ",") // TODO: exclude comma more elegantly
        {
            auto arg_expr1 = get_expr(a);

            stmt.args.push_back(a->getText());
            os << "MemberStatment arg ";
            auto* arg_expr = get_expr(a);
            // TODO: store arg_expr in stmt
            arg_expr->print(os);
            os << "\n";
        }
        else
        {
            std::cout << "Found comma\n";
        }
    }
    program->last_relation().add_statement(stmt);
}

lang::Expression const* ParseListener::get_expr(antlr4::tree::ParseTree* ctx)
{
    auto find_result = expr_map.find(ctx);

    if (find_result == expr_map.end())
    {
        std::cerr << "get_expr: " << ctx->getText() << "\n";
        throw std::logic_error("No expression for the requested parse context.");
    }

    return find_result->second;
}

void ParseListener::put_binop(std::string op, antlr4::ParserRuleContext* ctx)
{
    auto rhs_ = get_expr(ctx->children[2]);
    auto lhs_ = get_expr(ctx->children[0]);

    auto rhs = get_expr(ctx->children[2]);
    auto lhs = get_expr(ctx->children[0]);
    auto expr = new lang::BinOpExpr(op, lhs, rhs);
    put_expr(expr, ctx);

    log_database->update_expression_parent(rhs, expr);
    log_database->update_expression_parent(lhs, expr);
}

void ParseListener::exitDotExpr(ExprParser::DotExprContext* ctx)
{
    put_binop(".", ctx);
}

void ParseListener::exitIntExpr(ExprParser::IntExprContext* ctx)
{
    int value = std::stoi(ctx->getText());
    auto expr = new lang::IntExpr(value);
    put_expr(expr, ctx);
}

void ParseListener::exitQuasiquoteExpr(ExprParser::QuasiquoteExprContext* ctx)
{
    auto expr = new lang::QuasiquoteExpr();
    put_expr(expr, ctx);
}

void ParseListener::exitBinOpExpr2(ExprParser::BinOpExpr2Context* ctx)
{
    put_binop(ctx->op->getText(), ctx);
}

void ParseListener::exitBinOpExpr1(ExprParser::BinOpExpr1Context* ctx)
{
    put_binop(ctx->op->getText(), ctx);
}

void ParseListener::exitCallExpr(ExprParser::CallExprContext* ctx)
{
    auto method_name_ = get_expr(ctx->call_expression());

    std::string method_name = ctx->call_expression()
            ->call_target_name->getText();

    auto const& children = ctx
            ->call_expression()
            ->call_param_list()
            ->children;

    std::vector<lang::Expression const*> params;

    for (auto a : children)
    {
        if (a->getText() != ",") // TODO: exclude comma more elegantly
        {
            auto param = get_expr(a);
            params.push_back(param);
        }
    }

    auto expr = new lang::CallExpr(method_name, params);
    put_expr(expr, ctx);

    for (auto arg : params)
        log_database->update_expression_parent(arg, expr);
}

void ParseListener::exitParenExpr(ExprParser::ParenExprContext* ctx)
{
    auto inner_expr = get_expr(ctx->expr_expr());
    put_expr(inner_expr, ctx);
}

void ParseListener::exitIdExpr(ExprParser::IdExprContext* ctx)
{
    auto inner_expr = get_expr(ctx->expr_identifier());
    put_expr(inner_expr, ctx);
}

void ParseListener::print_exprs(std::ostream& os)
{
    for (auto kv : expr_map)
        os << *(kv.second) << "\n";
}

void ParseListener::put_expr(lang::Expression const* expr,
                             antlr4::ParserRuleContext* ctx)
{
    expr_map[ctx] = expr;

    log_database->insert_expression(expr, ctx);

    auto& os = std::cout;
    os << "\nAfter put, expr_map is now:\n";
    print_exprs(os);
}

void ParseListener::exitStat(ExprParser::StatContext*)
{
    expr_map.clear();
}

void ParseListener::exitExpr_identifier(ExprParser::Expr_identifierContext* ctx)
{
    std::string identifier = ctx->getText();
    auto expr = new lang::IdExpr(identifier);
    put_expr(expr, ctx);
}
