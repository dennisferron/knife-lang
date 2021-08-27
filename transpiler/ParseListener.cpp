#include "ParseListener.hpp"

ParseListener::ParseListener(ExprParser* parser, lang::Program* program, data::ParserRuleContextInserter* ctx_inserter)
    : parser(parser), program(program), ctx_inserter(ctx_inserter)
{
}

void ParseListener::enterEveryRule(antlr4::ParserRuleContext* ctx)
{
    ctx_inserter->insert(ctx);
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

lang::Expression const* ParseListener::pop_expr()
{
    if (expr_stack.empty())
        throw std::logic_error("Parsing attempted to pop empty expression stack.");

    auto result = *expr_stack.rbegin();
    expr_stack.pop_back();
    return result;
}

void ParseListener::push_binop(std::string op)
{
    auto rhs = pop_expr();
    auto lhs = pop_expr();
    auto expr = new lang::BinOpExpr(op, lhs, rhs);
    push_expr(expr);
}

void ParseListener::exitDotExpr(ExprParser::DotExprContext* ctx)
{
    push_binop(".");
}

void ParseListener::exitIntExpr(ExprParser::IntExprContext* ctx)
{
    int value = std::stoi(ctx->getText());
    auto expr = new lang::IntExpr(value);
    push_expr(expr);
}

void ParseListener::exitQuasiquoteExpr(ExprParser::QuasiquoteExprContext* ctx)
{
    auto expr = new lang::QuasiquoteExpr();
    push_expr(expr);
}

void ParseListener::exitBinOpExpr2(ExprParser::BinOpExpr2Context* ctx)
{
    push_binop(ctx->op->getText());
}

void ParseListener::exitBinOpExpr1(ExprParser::BinOpExpr1Context* ctx)
{
    push_binop(ctx->op->getText());
}

void ParseListener::exitCallExpr(ExprParser::CallExprContext* ctx)
{
    std::string method_name = ctx->call_expression()
            ->call_target_name->getText();

    // Need to count params from parsing to know how many
    // expressions to expect on stack.
    // TODO: should we use stack markers instead?
    int param_children = ctx->call_expression()
            ->call_param_list()
            ->children.size();

    // Ugly hack:  remove commas based on fencepost numbering.
    // () = 0:0; (a) = 1:1; (a,b) = 3:2; (a,b,c) = 5:3;
    // Add 1 and divide by 2 dropping remainder gives right result.
    int num_params = (1+param_children) / 2;

    std::vector<lang::Expression const*> params;

    for (int i=0; i<num_params; i++)
    {
        // Stack is LIFO; this re-reverses call params.
        params.insert(params.begin(), pop_expr());
    }

    auto expr = new lang::CallExpr(method_name, params);
    push_expr(expr);
}

void ParseListener::exitParenExpr(ExprParser::ParenExprContext* ctx)
{
    // Do nothing - the expr inside the parens is already on stack.
}

void ParseListener::exitIdExpr(ExprParser::IdExprContext* ctx)
{
    //ctx->start.
    std::string identifier = ctx->getText();
    auto expr = new lang::IdExpr(identifier);
    push_expr(expr);
}

void ParseListener::print_stack(std::ostream& os)
{
    for (auto expr : expr_stack)
        os << *expr << "\n";
}

void ParseListener::push_expr(lang::Expression const* expr)
{
    expr_stack.push_back(expr);
    auto& os = std::cerr;
    os << "\nAfter push, stack is now:\n";
    print_stack(os);
}