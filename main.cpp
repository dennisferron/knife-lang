#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 7)
    {
        std::cerr << "Usage: " << argv[0]
                  << " -o <output file>"
                  << " -t <template file>"
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
    else if (template_file.empty())
    {
        std::cerr << "Required: -t <template file>\n";
        return -1;
    }

    antlr4::ANTLRFileStream expr_strm(expr_file);
    ExprLexer lexer(&expr_strm);
    antlr4::CommonTokenStream tokens(&lexer);

    tokens.fill();
    for (auto token : tokens.getTokens()) {
        std::cout << token->toString() << std::endl;
    }

    ExprParser parser(&tokens);
    //antlr4::tree::ParseTree *tree = parser.stored_definition();
    //std::cout << tree->toStringTree(&parser) << std::endl;

    return 0;
}
