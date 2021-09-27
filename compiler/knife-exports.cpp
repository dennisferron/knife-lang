#define KNIFE_EXPORTS_CPP
#include "knife-exports.hpp"

#include "data/LogDatabase.hpp"
#include "data/NullLogger.hpp"
#include "KnifeParserContextImpl.hpp"
#include "CompilerImpl.hpp"
#include "OutputterImpl.hpp"

namespace knife
{
    knife::KnifeParserContext* create_file_parser(knife::data::ParseLogger* logger, std::string file_name)
    {
        auto knife_strm = new antlr4::ANTLRFileStream(file_name);

        if (knife_strm->size() == 0)
        {
            std::stringstream msg;
            msg << "Unable to open expr file (or file is empty): "
            << file_name << "\n";
            throw std::runtime_error(msg.str());
        }

        return new KnifeParserContextImpl(logger, knife_strm);
    }

    knife::KnifeParserContext* create_string_parser(std::string input_text, knife::data::ParseLogger* logger)
    {
        static knife::data::NullLogger null_logger;

        auto stream = new antlr4::ANTLRInputStream(input_text);
        return new KnifeParserContextImpl(
                logger ? logger : &null_logger,
                stream);
    }

    knife::data::ParseLogger* create_db_logger(std::string db_file_name)
    {
        return new data::LogDatabase(db_file_name);
    }

    knife::Compiler* create_compiler(knife::data::ParseLogger* logger)
    {
        return new CompilerImpl(logger);
    }

    knife::Outputter* create_outputter()
    {
        return new OutputterImpl();
    }
}