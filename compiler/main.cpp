#include "knife-exports.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <chrono>
#include <sstream>

// Generate a unique file name so parallel builds don't
// try to log to the same database file.
std::string generate_log_db_name(std::string base_name)
{
    std::stringstream ss;
    ss << base_name;
    ss << "_" << std::hex;

    {
        using namespace std::chrono;
        auto usec_since_epoch = duration_cast<microseconds>(
                system_clock::now().time_since_epoch()).count();
        ss << usec_since_epoch;
    }

    ss << ".db";
    return ss.str();
}


enum class ArgMode
{
    Default,
    Mode_o,
    Mode_i
};

using namespace knife;

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

    std::string compile_log_file = generate_log_db_name(output_base_name);
    data::ParseLogger* logger = create_db_logger(compile_log_file);

    lang::Program program;

    bool first_file = true;
    for (std::string input_file : input_files)
    {
        KnifeParserContext* parser_context = create_file_parser(logger, input_file);

        if (first_file)
        {
            parser_context->log_token_names();
            first_file = false;
        }

        // TODO: add to program instead of replacing it.
        program = parser_context->parse_program();
    }

    Compiler* compiler = create_compiler(logger);

    outp::ProgramNamespace prog_ns = compiler->transform(program);

    Outputter* outputter = create_outputter();

    std::ofstream ofs_header(output_base_name + "_header.json");
    outputter->output_header(ofs_header, prog_ns, output_base_name);
    ofs_header.close();

    std::ofstream ofs_source(output_base_name + "_source.json");
    outputter->output_source(ofs_source, prog_ns, output_base_name);
    ofs_source.close();

    std::ofstream ofs_db_init(output_base_name + "_db_init.json");
    outputter->output_db_init(ofs_db_init, prog_ns, output_base_name);
    ofs_db_init.close();

    std::cout << "Knife compiler finished." << std::endl;

    return 0;
}

