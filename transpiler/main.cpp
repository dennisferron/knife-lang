#include "antlr4-runtime.h"
#include "ExprLexer.h"
#include "ExprParser.h"
#include "ParseListener.hpp"

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
        std::cout << argv[i] << " ";
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
                    input_files.push_back(arg_str);
        }
    }

    if (output_base_name == "" ||
        input_folder == "" ||
        input_files.empty())
    {
        std::cerr << "Usage: " << argv[0]
                  << " -o <output base name>"
                  << " <one or more input files>\n";
        return -1;
    }

    std::string output_json_file = output_base_name + ".json";
    std::string output_stg_file = output_base_name + ".stg";

    for (std::string input_file : input_files)
    {
        std::string expr_file = input_folder + "\\" + input_file;

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
        antlr4::tree::ParseTree* expr_tree = expr_parser.prog();
        //std::cout << expr_tree->toStringTree(&expr_parser, true) << std::endl;

        antlr4::tree::ParseTreeWalker walker;
        ParseListener listener(&expr_parser);
        walker.walk(&listener, expr_tree); // initiate walk of tree with listener
    }

    // "Generate" code
    std::ofstream ofs(output_json_file);

    std::string data = R"(
{
	"className":"prog",

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
    ofs << data;
    ofs.close();

    std::string stg_data = R"(
database() ::= <<
CREATE TABLE IF NOT EXISTS foo (
    column1 INTEGER PRIMARY KEY,
    column2 TEXT
);
INSERT INTO foo (column1, column2) VALUES (1, "a")
    ON CONFLICT(column1) DO
UPDATE SET column2="aa";
INSERT INTO foo (column1, column2) VALUES (2, "b")
    ON CONFLICT(column1) DO
UPDATE SET column2="bb";
>>

header(vars, className) ::= <<
// Generated Header

class $className$
{
private:
	$vars:decl_var();separator="\n"$

public:
	int run();
};
>>


decl_var(v) ::= <<int $v$ = 0;>>

source(statements, className) ::= <<
// Generated Source

#include "$className$.hpp"

#include <iostream>

int $className$::run()
{
	$statements:select_statement();separator="\n"$
}

>>

select_statement(s) ::= <<$(s.template)(s.args)$;>>

print_statement(args) ::= <<std::cout << "$args.var$=" << $args.var$ << "\\n">>
assign_statement(args) ::= <<$args.assignee$ = $args.expr$>>
return_statement(args) ::= <<return $args.value$>>
    )";

    std::ofstream ofs_stg(output_stg_file);
    ofs_stg << stg_data;
    ofs_stg.close();

    return 0;
}
