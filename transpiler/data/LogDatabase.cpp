#include "LogDatabase.hpp"

#include <sstream>

using namespace data;

LogDatabase::LogDatabase(std::string db_file)
{
    int rc = sqlite3_open(db_file.c_str(), &db);

    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "Error opening database "
            << db_file << " Message: "
            << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error(msg.str());
    }

    create_tables();
}

LogDatabase::~LogDatabase()
{
    sqlite3_close(db);
}

void LogDatabase::begin_transaction()
{
    exec("BEGIN TRANSACTION");
}

void LogDatabase::commit_transaction()
{
    exec("COMMIT TRANSACTION");
}

void LogDatabase::rollback_transaction()
{
    exec("ROLLBACK TRANSACTION");
}

void LogDatabase::exec(std::string sql)
{
    char* errMsg;

    int rc = sqlite3_exec(
        db, /* An open database */
        sql.c_str(), /* SQL to be evaluated */
        nullptr,  /* Callback function (for results; not used here) */
        nullptr,  /* 1st argument to callback (not used here) */
        &errMsg  /* Error msg written here (must be freed later) */
    );

    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "SQL error: " << errMsg;
        msg << "\nWhile executing:\n" << sql;
        std::runtime_error exc(msg.str());
        sqlite3_free(errMsg);
        throw exc;
    }
}

void LogDatabase::create_tables()
{
    exec("DROP TABLE IF EXISTS TokenTypeNames;");
    exec(R"(
        CREATE TABLE TokenTypeNames (
            tokenType INTEGER PRIMARY KEY,
            tokenName TEXT
        );
    )");

    exec("DROP TABLE IF EXISTS Tokens;");
    exec(R"(
        CREATE TABLE IF NOT EXISTS Tokens (
            tokenIndex     INTEGER PRIMARY KEY,
            start          INTEGER,
            stop           INTEGER,
            tokenText      TEXT,
            tokenType      INTEGER,
            line           INTEGER,
            positionInLine TEXT
        );
    )");

    exec("DROP TABLE IF EXISTS ParserRuleContext;");
    exec(R"(
        CREATE TABLE ParserRuleContext (
            address    INTEGER PRIMARY KEY,
            parent     INTEGER,
            startToken INTEGER,
            stopToken  INTEGER
        );
    )");
}



TokenNamesInserter::TokenNamesInserter(const LogDatabase& db)
{
    char const* insert_sql = R"(
        insert into TokenTypeNames (
            tokenType, tokenName)
        values (?, ?))";

    int rc = sqlite3_prepare_v2(db.db, insert_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "Error preparing statement "
        << insert_sql << " Message: "
        << sqlite3_errmsg(db.db) << "\n";
        throw std::runtime_error(msg.str());
    }
}

TokenNamesInserter::~TokenNamesInserter()
{
    sqlite3_finalize(stmt);
}

void TokenNamesInserter::insert(std::size_t tokenType, std::string tokenName)
{
    sqlite3_bind_int64(stmt, 1, tokenType);
    sqlite3_bind_text(stmt, 2, tokenName.c_str(), tokenName.size(), SQLITE_TRANSIENT);

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}



TokenInserter::TokenInserter(const LogDatabase& db)
{
    char const* insert_sql = R"(
        insert into Tokens (
            tokenIndex,
            start, stop,
            tokenText, tokenType,
            line, positionInLine)
        values (?, ?, ?, ?, ?, ?, ?))";

    int rc = sqlite3_prepare_v2(db.db, insert_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "Error preparing statement "
        << insert_sql << " Message: "
        << sqlite3_errmsg(db.db) << "\n";
        throw std::runtime_error(msg.str());
    }
}

TokenInserter::~TokenInserter()
{
    sqlite3_finalize(stmt);
}

void TokenInserter::insert(antlr4::Token* token)
{
    sqlite3_bind_int64(stmt, 1, token->getTokenIndex());
    sqlite3_bind_int64(stmt, 2, token->getStartIndex());
    sqlite3_bind_int64(stmt, 3, token->getStopIndex());

    std::string tok_text = token->getText();
    sqlite3_bind_text(stmt, 4, tok_text.c_str(), tok_text.size(), SQLITE_TRANSIENT);

    sqlite3_bind_int64(stmt, 5, token->getType());

    sqlite3_bind_int64(stmt, 6, token->getLine());
    sqlite3_bind_int64(stmt, 7, token->getCharPositionInLine());

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}



ParserRuleContextInserter::ParserRuleContextInserter(const LogDatabase& db)
{
    char const* insert_sql = R"(
        insert into ParserRuleContext (
            address, parent,
            startToken, stopToken
        )
    values (?, ?, ?, ?))";

    int rc = sqlite3_prepare_v2(db.db, insert_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "Error preparing statement "
        << insert_sql << " Message: "
        << sqlite3_errmsg(db.db) << "\n";
        throw std::runtime_error(msg.str());
    }
}

ParserRuleContextInserter::~ParserRuleContextInserter()
{
    sqlite3_finalize(stmt);
}

void ParserRuleContextInserter::insert(antlr4::ParserRuleContext* ctx)
{
    sqlite3_bind_int64(stmt, 1, reinterpret_cast<sqlite3_int64>(ctx));
    sqlite3_bind_int64(stmt, 2, reinterpret_cast<sqlite3_int64>(ctx->parent));
    sqlite3_bind_int64(stmt, 3, ctx->getStart()->getTokenIndex());
    sqlite3_bind_int64(stmt, 4, ctx->getStop()->getTokenIndex());

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}
