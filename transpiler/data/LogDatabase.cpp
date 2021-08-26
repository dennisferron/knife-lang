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
    exec("DROP TABLE IF EXISTS ParserRuleContext;");
    exec(R"(
        CREATE TABLE ParserRuleContext (
            address    INTEGER PRIMARY KEY,
            startToken INTEGER,
            stopToken  INTEGER
        );
    )");

    exec("DROP TABLE IF EXISTS Tokens;");
    exec(R"(
        CREATE TABLE IF NOT EXISTS Tokens (
            tokenIndex     INTEGER PRIMARY KEY,
            line           INTEGER,
            positionInLine TEXT
        );
    )");
}

TokenInserter::TokenInserter(const LogDatabase& db)
{
    char const* insert_sql = R"(
        insert into Tokens (tokenIndex, line, positionInLine)
        values (?, ?, ?))";

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
    sqlite3_bind_int64(stmt, 2, token->getLine());
    sqlite3_bind_int64(stmt, 3, token->getCharPositionInLine());

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}
