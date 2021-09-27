#include "KnifeParserContextImpl.hpp"
#include "knife-exports.hpp"

using namespace knife;

KnifeParserContextImpl::KnifeParserContextImpl(data::ParseLogger* logger, antlr4::CharStream* input)
: logger(logger), knife_lexer(input), knife_tokens(&knife_lexer)
{
}

void KnifeParserContextImpl::parse(
        std::function<
            antlr4::tree::ParseTree*(KnifeParser&)
        > rule_selector
    )
{
    knife_lexer.removeErrorListeners();
    knife_lexer.addErrorListener(&knife_error_listener);

    antlr4::CommonTokenStream knife_tokens(&knife_lexer);
    knife_tokens.fill();

    logger->begin_transaction();
    for (auto knife_token : knife_tokens.getTokens())
    {
        //std::cout << knife_token->toString() << std::endl;
        logger->insert_token(knife_token);
    }
    logger->commit_transaction();

    knife_parser.reset(new KnifeParser(&knife_tokens));

    knife_parser->removeErrorListeners(); // remove ConsoleErrorListener
    knife_parser->addErrorListener(&knife_error_listener);

    listener.reset(new ParseListener(knife_parser.get(), logger));

    auto start_rule = rule_selector(*knife_parser);

    antlr4::tree::ParseTreeWalker walker;
    walker.walk(listener.get(), start_rule); // initiate walk of tree with listener

    if (auto errs = knife_parser->getNumberOfSyntaxErrors())
    {
        std::stringstream msg;
        msg << errs << " syntax errors." << std::endl;
        throw std::runtime_error(msg.str());
    }
}

void KnifeParserContextImpl::log_token_names()
{
    logger->begin_transaction();
    auto const& tok_names = knife_lexer.getTokenNames();

    for (std::size_t i=0; i<tok_names.size(); i++)
    {
        std::string name = tok_names[i];
        logger->insert_token_name(i, name);
    }

    logger->commit_transaction();

    logger->begin_transaction();
    auto const& rule_names = knife_lexer.getRuleNames();

    for (std::size_t i=0; i<rule_names.size(); i++)
    {
        std::string name = rule_names[i];
        logger->insert_rule_name(i, name);
    }

    logger->commit_transaction();
}

lang::Program KnifeParserContextImpl::parse_program()
{
    parse([](KnifeParser& parser){ return parser.prog(); });
    return listener->get_program();
}

lang::Expression const& KnifeParserContextImpl::parse_expression()
{
    parse([](auto& parser){ return parser.expr(); });
    return listener->get_root_expr();
}
