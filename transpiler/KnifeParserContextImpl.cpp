#include "KnifeParserContextImpl.hpp"
#include "knife-exports.hpp"

using namespace knife;

KnifeParserContextImpl::KnifeParserContextImpl(data::ParseLogger* logger, antlr4::CharStream* input)
    : logger(logger), knife_lexer(input)
{
}

lang::Program KnifeParserContextImpl::parse()
{
    CerrErrorListener knife_error_listener;
    knife_lexer.removeErrorListeners();
    knife_lexer.addErrorListener(&knife_error_listener);

    antlr4::CommonTokenStream knife_tokens(&knife_lexer);

    logger->begin_transaction();

    knife_tokens.fill();
    for (auto knife_token : knife_tokens.getTokens())
    {
        //std::cout << knife_token->toString() << std::endl;
        logger->insert_token(knife_token);
    }

    logger->commit_transaction();

    knife_parser.reset(new KnifeParser(&knife_tokens));
    antlr4::tree::ParseTree* knife_tree = knife_parser->prog();
    //std::cout << knife_tree->toStringTree(&knife_parser, true) << std::endl;

    knife_parser->removeErrorListeners(); // remove ConsoleErrorListener
    knife_parser->addErrorListener(&knife_error_listener);

    // TODO: This could be lang::TranslationUnit owned by listener.
    lang::Program program;
    ParseListener listener(knife_parser.get(), &program, logger);

    antlr4::tree::ParseTreeWalker walker;
    walker.walk(&listener, knife_tree); // initiate walk of tree with listener

    if (auto errs = knife_parser->getNumberOfSyntaxErrors())
    {
        std::stringstream msg;
        msg << errs << " syntax errors." << std::endl;
        throw std::runtime_error(msg.str());
    }

    return program;
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
