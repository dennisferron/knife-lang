#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "ParseListener.hpp"
#include "outp/Output.hpp"
#include "Transform.hpp"

#include <iostream>
#include <string>

// We want to be warned about ambiguous parses while developing grammar,
// so using DiagnosticErrorListener in place of BaseErrorListener.
class CerrErrorListener : public antlr4::DiagnosticErrorListener
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
    }
};

enum class ArgMode
{
    Default,
    Mode_o,
    Mode_i
};

int main(int argc, char* argv[])
{
    std::cout << "Command line:\n";
    for (int i=0; i<argc; i++)
    {
        std::cout << "<" << argv[i] << "> ";
    }
    std::cout << std::endl;

    std::string output_base_name = "";
    std::string input_folder = "";
    std::vector<std::string> input_files;

    ArgMode mode = ArgMode::Default;
    for (int i=1; i<argc; i++)
    {
        std::string arg_str = argv[i];
        switch (mode)
        {
            case ArgMode::Mode_o:
                output_base_name = arg_str;
                mode = ArgMode::Default;
                break;
            case ArgMode::Mode_i:
                input_folder = arg_str;
                mode = ArgMode::Default;
                break;
            default:
                if (arg_str == "-o")
                    mode = ArgMode::Mode_o;
                else if (arg_str == "-i")
                    mode = ArgMode::Mode_i;
                else if (arg_str[0] == '-')
                {
                    std::cerr << "Unrecognized option " << arg_str << "\n";
                    return -1;
                }
                else
                {
                    std::string full_path = input_folder + "/" + arg_str;
                    input_files.push_back(full_path);
                }
        }
    }

    if (output_base_name == "" ||
        input_files.empty())
    {
        std::cerr << "Usage: " << argv[0]
                  << " -o <output base name>"
                  << "[-i input file path]"
                  << " <one or more input files>\n";
        return -1;
    }

    // Information gleaned by the parse listener on all
    // of the input files will be accumulated in this object.
    lang::Program program;

    for (std::string expr_file : input_files)
    {
        antlr4::ANTLRFileStream expr_strm(expr_file);

        if (expr_strm.size() == 0)
        {
            std::cerr << "Unable to open expr file (or file is empty): "
                      << expr_file << "\n";
            return -1;
        }

        ExprLexer expr_lexer(&expr_strm);

        CerrErrorListener expr_error_listener;
        expr_lexer.removeErrorListeners();
        expr_lexer.addErrorListener(&expr_error_listener);

        antlr4::CommonTokenStream expr_tokens(&expr_lexer);

        //    expr_tokens.fill();
        //    for (auto expr_token : expr_tokens.getTokens())
        //    {
        //        std::cout << expr_token->toString() << std::endl;
        //    }

        ExprParser expr_parser(&expr_tokens);
        antlr4::tree::ParseTree* expr_tree = expr_parser.prog();
        //std::cout << expr_tree->toStringTree(&expr_parser, true) << std::endl;

        expr_parser.removeErrorListeners(); // remove ConsoleErrorListener
        expr_parser.addErrorListener(&expr_error_listener);

        antlr4::tree::ParseTreeWalker walker;
        ParseListener listener(&expr_parser, &program);
        walker.walk(&listener, expr_tree); // initiate walk of tree with listener

        if (auto errs = expr_parser.getNumberOfSyntaxErrors())
        {
            std::cerr << errs << " syntax errors." << std::endl;
            return -1;
        }
    }

    outp::ProgramNamespace program_output = transform(program);

    OutputHeader(output_base_name).write(program_output);
    OutputSource(output_base_name).write(program_output);
    OutputDbInit(output_base_name).write(program_output);

    std::cout << "Transpiler finished." << std::endl;

    return 0;
}

