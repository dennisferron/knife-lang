#include "LogDatabase.hpp"

#include <sstream>

using namespace knife;
using namespace knife::data;

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
    prepare_statements();
}

LogDatabase::~LogDatabase()
{
    sqlite3_finalize(stmt_insert_token_names);
    sqlite3_finalize(stmt_insert_token);
    sqlite3_finalize(stmt_insert_parse_context);
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

sqlite3_stmt* LogDatabase::prepare(const char* sql)
{
    sqlite3_stmt* result = nullptr;

    int rc = sqlite3_prepare_v2(db, sql, -1, &result, nullptr);
    if (rc != SQLITE_OK)
    {
        std::stringstream msg;
        msg << "Error preparing statement "
        << sql << " Message: "
        << sqlite3_errmsg(db) << "\n";
        throw std::runtime_error(msg.str());
    }
    else
    {
        return result;
    }
}

void LogDatabase::create_tables()
{
    exec("DROP TABLE IF EXISTS TokenTypeNames;");
    exec(R"(
        CREATE TABLE IF NOT EXISTS TokenTypeNames (
            tokenType INTEGER PRIMARY KEY,
            tokenName TEXT
        );
    )");

    exec("DROP TABLE IF EXISTS RuleNames;");
    exec(R"(
        CREATE TABLE IF NOT EXISTS RuleNames (
            ruleIndex INTEGER PRIMARY KEY,
            ruleName TEXT
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
        CREATE TABLE IF NOT EXISTS ParserRuleContext (
            address    INTEGER PRIMARY KEY,
            parent     INTEGER,
            startToken INTEGER,
            stopToken  INTEGER,
            ruleIndex  INTEGER
        );
    )");

    exec("DROP TABLE IF EXISTS Expressions;");
    exec(R"(
        CREATE TABLE IF NOT EXISTS Expressions (
            address      INTEGER,
            parent       INTEGER,
            parseContext INTEGER,
            description  TEXT
        );
    )");

    exec("DROP VIEW IF EXISTS ExpressionsView;");
    exec(R"(
    CREATE VIEW IF NOT EXISTS ExpressionsView AS
        SELECT tk.line,
               rn.ruleName,
               e.description
          FROM Expressions e
               JOIN
               ParserRuleContext c ON e.parseContext == c.address
               JOIN
               RuleNames rn ON rn.ruleIndex == c.ruleIndex
               JOIN
               Tokens tk ON tk.tokenIndex == c.startToken;
    )");
}

void LogDatabase::prepare_statements()
{
    stmt_insert_token_names = prepare(R"(
        insert into TokenTypeNames (
            tokenType, tokenName)
        values (?, ?)
    )");

    stmt_insert_rule_names = prepare(R"(
        insert into RuleNames (
            ruleIndex, ruleName)
        values (?, ?)
    )");

    stmt_insert_token = prepare(R"(
        insert into Tokens (
            tokenIndex,
            start, stop,
            tokenText, tokenType,
            line, positionInLine)
        values (?, ?, ?, ?, ?, ?, ?)
    )");

    stmt_insert_parse_context = prepare(R"(
        insert into ParserRuleContext (
            address, parent,
            startToken, stopToken,
            ruleIndex
        )
        values (?, ?, ?, ?, ?)
    )");

    stmt_insert_expression = prepare(R"(
        insert into Expressions (
            address,
            parseContext, description
        )
        values (?, ?, ?)
    )");

    stmt_update_expression_parent = prepare(R"(
        update Expressions
            set parent = ?
            where address = ?
    )");
}

void LogDatabase::insert_token_name(std::size_t tokenType, std::string tokenName)
{
    auto stmt = stmt_insert_token_names;

    sqlite3_bind_int64(stmt, 1, tokenType);
    sqlite3_bind_text(stmt, 2, tokenName.c_str(),
                      tokenName.size(), SQLITE_TRANSIENT);

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}

void LogDatabase::insert_rule_name(std::size_t rule_index, std::string rule_name)
{
    auto stmt = stmt_insert_rule_names;

    sqlite3_bind_int64(stmt, 1, rule_index);
    sqlite3_bind_text(stmt, 2, rule_name.c_str(),
                      rule_name.size(), SQLITE_TRANSIENT);

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}

void LogDatabase::insert_token(antlr4::Token* token)
{
    auto stmt = stmt_insert_token;

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

void LogDatabase::insert_parse_context(antlr4::ParserRuleContext* ctx)
{
    auto stmt = stmt_insert_parse_context;

    sqlite3_bind_int64(stmt, 1, reinterpret_cast<sqlite3_int64>(ctx));
    sqlite3_bind_int64(stmt, 2, reinterpret_cast<sqlite3_int64>(ctx->parent));
    sqlite3_bind_int64(stmt, 3, ctx->getStart()->getTokenIndex());
    sqlite3_bind_int64(stmt, 4, ctx->getStop()->getTokenIndex());
    sqlite3_bind_int64(stmt, 5, ctx->getRuleIndex());

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}

void LogDatabase::insert_expression(lang::Expression const* expr, antlr4::ParserRuleContext const* ctx)
{
    auto stmt = stmt_insert_expression;

    sqlite3_bind_int64(stmt, 1,
                       reinterpret_cast<sqlite3_int64>(expr));

    sqlite3_bind_int64(stmt, 2,
                       reinterpret_cast<sqlite3_int64>(ctx));

    std::stringstream desc; desc << *expr;
    sqlite3_bind_text(stmt, 3, desc.str().c_str(),
                     desc.str().size(), SQLITE_TRANSIENT);

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}

void LogDatabase::update_expression_parent(lang::Expression const* expr, lang::Expression const* parent)
{
    auto stmt = stmt_update_expression_parent;

    sqlite3_bind_int64(stmt, 1,
                       reinterpret_cast<sqlite3_int64>(parent));
    sqlite3_bind_int64(stmt, 2,
                       reinterpret_cast<sqlite3_int64>(expr));

    sqlite3_step(stmt);

    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
}

