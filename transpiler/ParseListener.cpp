#include "ParseListener.hpp"

using namespace knife;

ParseListener::ParseListener(KnifeParser* parser, lang::Program* program, data::ParseLogger* logger)
    : parser(parser), program(program), logger(logger)
{
}

void ParseListener::enterEveryRule(antlr4::ParserRuleContext* ctx)
{
    logger->insert_parse_context(ctx);
}

//void ParseListener::enterSql_stmt_list(KnifeParser::Sql_stmt_listContext *ctx)
//{
//    auto tokens = parser->getTokenStream();
//    std::string text = tokens->getText(ctx);
//    std::cout << "SQL statements: "
//        << text << "\n";
//
//    //System.out.println("\t"+type+" "+ctx.Identifier()+args+";");
//}

void ParseListener::enterQuasiQuoteSql(KnifeParser::QuasiQuoteSqlContext* ctx)
{
    auto tokens = parser->getTokenStream();
    //std::string text = tokens->getText(ctx->sql_stmt_list());
    //std::cout << "SQL statements: "
    //          << text << "\n";
}

void ParseListener::enterQuasiQuoteCsv(KnifeParser::QuasiQuoteCsvContext* ctx)
{
    auto tokens = parser->getTokenStream();
    std::string text = tokens->getText(ctx->csv_row_list());
    std::cout << "CSV rows: "
              << text << "\n";
}

void ParseListener::enterRelation_name(KnifeParser::Relation_nameContext* ctx)
{
    std::string id = ctx->IDENT()->getText();
    program->relations.push_back(lang::Relation(id));
}

void ParseListener::enterRelation_param(KnifeParser::Relation_paramContext* ctx)
{
    std::string name = ctx->IDENT(0)->getText();
    std::string type = "";
    if (ctx->IDENT(1))
        type = ctx->IDENT(1)->getText();
    program->last_relation().add_param(lang::ParamVar {name, type});
}

void ParseListener::enterLet_stmt(KnifeParser::Let_stmtContext* ctx)
{
    lang::LetStatement stmt =
    {
        ctx->var_name->getText(),
        ctx->var_type ? ctx->var_type->getText() : std::string(""),
        ctx->var_init->getText()  // TODO: get the expr
    };

    program->last_relation().add_statement(stmt);
}

void ParseListener::enterFresh_stmt(KnifeParser::Fresh_stmtContext* ctx)
{
    lang::FreshStatement stmt =
    {
        ctx->var_name->getText(),
        ctx->var_type ? ctx->var_type->getText() : ""
    };

    program->last_relation().add_statement(stmt);
}

void ParseListener::enterYield_stmt(KnifeParser::Yield_stmtContext* ctx)
{
    // TODO: get the expr
    program->last_relation().add_statement(lang::YieldStatement());
}

void ParseListener::exitMember_stmt(KnifeParser::Member_stmtContext* ctx)
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
            stmt.args.push_back(a->getText());
            //os << "MemberStatment arg ";
            auto* arg_expr = get_expr(
                static_cast<antlr4::ParserRuleContext*>(a));
            // TODO: store arg_expr in stmt
            //arg_expr->print(os);
            //os << "\n";
        }
        else
        {
            //std::cout << "Found comma\n";
        }
    }
    program->last_relation().add_statement(stmt);
}

lang::Expression const* ParseListener::get_expr(antlr4::ParserRuleContext* ctx)
{
    auto find_result = expr_map.find(ctx);

    if (find_result == expr_map.end())
    {
        std::stringstream msg;
        msg << "No expression for the requested parse context."
            << "\nLine " << ctx->start->getLine()
            << ":" << ctx->start->getCharPositionInLine()
            << "\n Text " << ctx->getText()
            << "\n Rule " << parser->getRuleNames()[ctx->getRuleIndex()]
                ;

        auto p = ctx;
        while (p = dynamic_cast<antlr4::ParserRuleContext*>(p->parent))
        {
            msg << "\n in " << parser->getRuleNames()[p->getRuleIndex()]
            ;
        }

        throw std::logic_error(msg.str());
    }

    return find_result->second;
}

void ParseListener::put_binop(std::string op,
                              antlr4::ParserRuleContext* ctx,
                              antlr4::ParserRuleContext* lhs,
                              antlr4::ParserRuleContext* rhs)
{
    auto lhs_expr = get_expr(lhs);
    auto rhs_expr = get_expr(rhs);
    auto expr = new lang::BinOpExpr(op, lhs_expr, rhs_expr);
    put_expr(expr, ctx);

    logger->update_expression_parent(lhs_expr, expr);
    logger->update_expression_parent(rhs_expr, expr);
}

void ParseListener::exitDotExpr(KnifeParser::DotExprContext* ctx)
{
    put_binop(".", ctx, ctx->lhs, ctx->rhs);
}

void ParseListener::exitIntExpr(KnifeParser::IntExprContext* ctx)
{
    int value = std::stoi(ctx->getText());
    auto expr = new lang::IntExpr(value);
    put_expr(expr, ctx);
}

void ParseListener::exitQuasiquoteExpr(KnifeParser::QuasiquoteExprContext* ctx)
{
    auto expr = new lang::QuasiquoteExpr();
    put_expr(expr, ctx);
}

void ParseListener::exitBinOpExpr2(KnifeParser::BinOpExpr2Context* ctx)
{
    put_binop(ctx->op->getText(), ctx, ctx->lhs, ctx->rhs);
}

void ParseListener::exitBinOpExpr1(KnifeParser::BinOpExpr1Context* ctx)
{
    put_binop(ctx->op->getText(), ctx, ctx->lhs, ctx->rhs);
}

void ParseListener::exitCallExpr(KnifeParser::CallExprContext* ctx)
{
    auto inner_expr = get_expr(ctx->call_expression());
    put_expr(inner_expr, ctx);
}

void ParseListener::exitCall_expression(KnifeParser::Call_expressionContext* ctx)
{
    std::string method_name = ctx->call_target_name->getText();

    auto const& children = ctx
            ->call_param_list()
            ->children;

    std::vector<lang::Expression const*> params;

    for (auto a : children)
    {
        if (a->getText() != ",") // TODO: exclude comma more elegantly
        {
            auto param = get_expr(
                static_cast<antlr4::ParserRuleContext*>(a));
            params.push_back(param);
        }
    }

    auto expr = new lang::CallExpr(method_name, params);
    put_expr(expr, ctx);

    for (auto arg : params)
        logger->update_expression_parent(arg, expr);
}

void ParseListener::exitParenExpr(KnifeParser::ParenExprContext* ctx)
{
    auto inner_expr = get_expr(ctx->expr());
    put_expr(inner_expr, ctx);
}

void ParseListener::exitIdExpr(KnifeParser::IdExprContext* ctx)
{
    auto inner_expr = get_expr(ctx->identifier());
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

    logger->insert_expression(expr, ctx);

    //auto& os = std::cout;
    //os << "\nAfter put, expr_map is now:\n";
    //print_exprs(os);
}

void ParseListener::exitStat(KnifeParser::StatContext*)
{
    expr_map.clear();
}

void ParseListener::exitIdentifier(KnifeParser::IdentifierContext* ctx)
{
    std::string identifier = ctx->getText();
    auto expr = new lang::IdExpr(identifier);
    put_expr(expr, ctx);
}
