#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "Subst.h"

#include <iostream>
#include <string>

class CerrErrorListener : public antlr4::BaseErrorListener
{
    virtual void syntaxError(
            antlr4::Recognizer* recognizer,
            antlr4::Token* offendingSymbol,
            size_t line,
            size_t charPositionInLine,
            const std::string& msg,
            std::exception_ptr e) override
    {
        std::cerr << "Error on Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ")";
        //throw std::invalid_argument(s.str());
    }
};

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0]
                  << " -o <output file>"
                  //<< " -t <template file>"
                  << " -e <expr file>\n";
        return -1;
    }

    std::string output_file;
    std::string expr_file;
    std::string template_file;

    for (int i=1; i<argc; i+=2)
    {
        std::string option = argv[i];
        std::string value = argv[i+1];

        if (option == "-o")
            output_file = value;
        else if (option == "-e")
            expr_file = value;
        else if (option == "-t")
            template_file = value;
        else
        {
            std::cerr << "Invalid option: " << option << "\n";
            return -1;
        }
    }

    if (output_file.empty())
    {
        std::cerr << "Required: -o <output file>\n";
        return -1;
    }
    else if (expr_file.empty())
    {
        std::cerr << "Required: -e <expr file>\n";
        return -1;
    }
//    else if (template_file.empty())
//    {
//        std::cerr << "Required: -t <template file>\n";
//        return -1;
//    }

    antlr4::ANTLRFileStream expr_strm(expr_file);

    if (expr_strm.size() == 0)
    {
        std::cerr << "Unable to open expr file (or file is empty): "
            << expr_file << "\n";
        return -1;
    }

    ExprLexer expr_lexer(&expr_strm);

    CerrErrorListener expr_error_listner;
    expr_lexer.removeErrorListeners();
    expr_lexer.addErrorListener(&expr_error_listner);

    antlr4::CommonTokenStream expr_tokens(&expr_lexer);

//    expr_tokens.fill();
//    for (auto expr_token : expr_tokens.getTokens())
//    {
//        std::cout << expr_token->toString() << std::endl;
//    }

    ExprParser expr_parser(&expr_tokens);
    antlr4::tree::ParseTree *expr_tree = expr_parser.prog();
    std::cout << expr_tree->toStringTree(&expr_parser, true) << std::endl;

    /*
    std::cout << "\nSubst lexer:\n";

    antlr4::ANTLRFileStream template_strm(template_file);

    if (template_strm.size() == 0)
    {
        std::cerr << "Unable to open template file (or file is empty): "
                  << template_file << "\n";
        return -1;
    }

    std::cout << "Template file size: " << template_strm.size() << "\n";
    std::cout << "Template file: " << template_strm.toString() << "\n";

    Subst subst_lexer(&template_strm);

    CerrErrorListener subst_error_listner;
    subst_lexer.removeErrorListeners();
    subst_lexer.addErrorListener(&subst_error_listner);
    
    antlr4::CommonTokenStream subst_tokens(&subst_lexer);

    subst_tokens.fill();
    for (auto subst_token : subst_tokens.getTokens())
    {
        switch (subst_token->getType())
        {
            case Subst::RawText:
                std::cout << "RawText: "
                    << subst_token->getText()
                    << "\n";
                break;
            case Subst::Identifier:
                std::cout << "Identifier: "
                    << subst_token->getText()
                    << "\n";
                break;
            default:
                // Do nothing
                break;
        }
    }
     */

    // "Generate" code
    std::ofstream ofs(output_file);

    std::string data = R"(
    {
       "application": "hiking",
       "reputons": [
       {
           "rater": "HikingAsylum",
           "assertion": "advanced",
           "rated": "Marilyn C",
           "rating": 0.90,
           "confidence": 0.99
         }
       ]
    }
)";
    ofs << data;
    ofs.close();

    return 0;
}
