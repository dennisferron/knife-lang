#include "antlr4-runtime.h"
#include "KnifeLexer.h"
#include "KnifeParser.h"
#include "ParseListener.hpp"
#include "outp/Output.hpp"
#include "Transform.hpp"
#include "data/LogDatabase.hpp"

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

    std::string compile_log_file = output_base_name + "-compile.db";
    data::LogDatabase db(compile_log_file);

    // Information gleaned by the parse listener on all
    // of the input files will be accumulated in this object.
    lang::Program program;

    bool first_file = true;

    for (std::string knife_file : input_files)
    {
        antlr4::ANTLRFileStream knife_strm(knife_file);

        if (knife_strm.size() == 0)
        {
            std::cerr << "Unable to open expr file (or file is empty): "
                      << knife_file << "\n";
            return -1;
        }

        KnifeLexer knife_lexer(&knife_strm);

        if (first_file)
        {
            db.begin_transaction();
            auto const& tok_names = knife_lexer.getTokenNames();

            for (std::size_t i=0; i<tok_names.size(); i++)
            {
                std::string name = tok_names[i];
                db.insert_token_name(i, name);
            }

            db.commit_transaction();

            db.begin_transaction();
            auto const& rule_names = knife_lexer.getRuleNames();

            for (std::size_t i=0; i<rule_names.size(); i++)
            {
                std::string name = rule_names[i];
                db.insert_rule_name(i, name);
            }

            db.commit_transaction();

            first_file = false;
        }

        CerrErrorListener knife_error_listener;
        knife_lexer.removeErrorListeners();
        knife_lexer.addErrorListener(&knife_error_listener);

        antlr4::CommonTokenStream knife_tokens(&knife_lexer);

        db.begin_transaction();

        knife_tokens.fill();
        for (auto knife_token : knife_tokens.getTokens())
        {
            std::cout << knife_token->toString() << std::endl;
            db.insert_token(knife_token);
        }

        db.commit_transaction();

        KnifeParser knife_parser(&knife_tokens);
        antlr4::tree::ParseTree* knife_tree = knife_parser.prog();
        //std::cout << knife_tree->toStringTree(&knife_parser, true) << std::endl;

        knife_parser.removeErrorListeners(); // remove ConsoleErrorListener
        knife_parser.addErrorListener(&knife_error_listener);

        ParseListener listener(&knife_parser, &program, &db);

        antlr4::tree::ParseTreeWalker walker;
        walker.walk(&listener, knife_tree); // initiate walk of tree with listener

        if (auto errs = knife_parser.getNumberOfSyntaxErrors())
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

