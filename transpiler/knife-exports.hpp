#pragma once

#ifdef _WIN32
    #ifdef KNIFE_LIB_STATIC
        #define KNIFE_API
    #else
        #ifdef KNIFE_EXPORTS_CPP
            #define KNIFE_API __declspec(dllexport)
        #else
            #define KNIFE_API __declspec(dllimport)
        #endif
    #endif
#else
    #define KNIFE_API
#endif

#include "KnifeParserContext.hpp"
#include "Compiler.hpp"
#include "Outputter.hpp"

namespace knife
{
    namespace data
    {
        class ParseLogger;
    }

    KNIFE_API knife::data::ParseLogger* create_db_logger(std::string db_file_name);
    KNIFE_API knife::KnifeParserContext* create_file_parser(knife::data::ParseLogger* logger, std::string file_name);
    KNIFE_API knife::KnifeParserContext* create_string_parser(knife::data::ParseLogger* logger, std::string input_text);
    KNIFE_API knife::Compiler* create_compiler(knife::data::ParseLogger* logger);
    KNIFE_API knife::Outputter* create_outputter();
}
