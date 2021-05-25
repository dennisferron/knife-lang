#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "ParseListener.hpp"

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

void output_header(std::string output_base_name)
{
    std::string output_json_file = output_base_name + "_header.json";
    std::ofstream ofs(output_json_file);

    ofs << "{\n";
    ofs << R"( "className":")" << output_base_name << R"(", )";
    ofs << R"(
	"vars":[
		"p",
		"c"
	],

	"statements":[
		{ "template":"assign_statement", "args":{"assignee":"x", "expr":"2+b"}},
		{ "template":"print_statement", "args":{"var":"x"}},
		{ "template":"return_statement", "args":{"value":42}}
	]
})";
    ofs << "}";

    ofs.close();
}

void output_source(std::string output_base_name)
{
    std::string output_json_file = output_base_name + "_source.json";
    std::ofstream ofs(output_json_file);

    ofs << "{\n";
    ofs << R"( "className":")" << output_base_name << R"(", )";
    ofs << R"(
	"vars":[
		"p",
		"c"
	],

	"statements":[
		{ "template":"assign_statement", "args":{"assignee":"x", "expr":"2+b"}},
		{ "template":"print_statement", "args":{"var":"x"}},
		{ "template":"return_statement", "args":{"value":42}}
	]
})";
    ofs << "}";

    ofs.close();
}

void output_db_init(std::string output_base_name)
{
    std::string output_json_file = output_base_name + "_db_init.json";
    std::ofstream ofs(output_json_file);

    ofs << "{\n";

    //ofs << R"( "className":")" << output_base_name << R"(", )";
    ofs << R"(
	"vars":[
		"b",
		"x"
	],

	"statements":[
		{ "template":"assign_statement", "args":{"assignee":"x", "expr":"2+b"}},
		{ "template":"print_statement", "args":{"var":"x"}},
		{ "template":"return_statement", "args":{"value":42}}
	]
})";
    ofs << "}";

    ofs.close();
}

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
        ParseListener listener(&expr_parser);
        walker.walk(&listener, expr_tree); // initiate walk of tree with listener

        if (auto errs = expr_parser.getNumberOfSyntaxErrors())
        {
            std::cerr << errs << " syntax errors." << std::endl;
            return -1;
        }
    }

    output_header(output_base_name);
    output_source(output_base_name);
    output_db_init(output_base_name);

    std::cout << "Transpiler finished." << std::endl;

    return 0;
}

