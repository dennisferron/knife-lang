/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 by Bart Kiers
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Project : sqlite-parser; an ANTLR4 grammar for SQLite https://github.com/bkiers/sqlite-parser
 * Developed by : Bart Kiers, bart@big-o.nl Martin Mirchev, marti_2203@abv.bg
 */
parser grammar ExprParser;

options {
	tokenVocab = ExprLexer;
}

/** The start rule; begin parsing here. */
prog:   relation* EOF ;

relation: relation_header EXPR_OPEN_BRACE stat* EXPR_CLOSE_BRACE;

relation_header: EXPR_RELATION relation_name EXPR_OPEN_PAR relation_param_list? EXPR_CLOSE_PAR;
relation_name: EXPR_ID;
relation_param_list: relation_param (EXPR_COMMA relation_param)*;
relation_param: EXPR_ID (EXPR_COLON EXPR_ID)?;

stat: (
        let_stmt
    |   fresh_stmt
    |   yield_stmt
    |   expr2
    |   EXPR_ID EXPR_ASGN expr2
    ) EXPR_SEMICOLON
    ;

let_stmt: EXPR_LET EXPR_ID type_annotation? EXPR_ASGN expr2;
fresh_stmt: EXPR_FRESH EXPR_ID type_annotation?;
yield_stmt: EXPR_YIELD expr2;

type_annotation: EXPR_COLON EXPR_ID;

expr2:
	   EXPR_OPEN_QUASIQUOTE quasiquote
    |  expr2 (EXPR_MULT|EXPR_DIV|EXPR_AMP) expr2
    |   expr2 (EXPR_PLUS|EXPR_MINUS|EXPR_BAR) expr2
    |  call_expression
    |   EXPR_INT                    
    |   EXPR_ID                    
    |   EXPR_OPEN_PAR expr2 EXPR_CLOSE_PAR
    |   EXPR_ID EXPR_DOT EXPR_ID
    ;

call_expression: EXPR_ID EXPR_OPEN_PAR call_param_list EXPR_CLOSE_PAR;
call_param_list: call_param (EXPR_COMMA call_param)*;
call_param: expr2;

quasiquote:
	   QUASIQUOTE_SQL sql_stmt_list SQL_CLOSE_QUASIQUOTE #quasiQuoteSql
	|  QUASIQUOTE_CSV csv_header csv_row_list CSV_CLOSE_QUASIQUOTE #quasiQuoteCsv
    ;

csv_header: CSV_HDR_OPEN_PAR csv_params_list CSV_HDR_CLOSE_PAR CSV_HDR_NEWLINE;
csv_params_list: csv_param (CSV_HDR_COMMA csv_param)*;
csv_param: CSV_HDR_ID (CSV_HDR_COLON CSV_HDR_ID)?;
csv_row_list: csv_row+;
csv_row: csv_field (CSV_COMMA csv_field)* CSV_NEWLINE;
csv_field:  CSV_TEXT #csvTextField
        |   CSV_STRING #csvStringField
        |   #csvEmptyField
        ;

sql_stmt_list: SQL_SEMICOL* sql_stmt (SQL_SEMICOL+ sql_stmt)* SQL_SEMICOL*;

sql_stmt: (EXPLAIN (QUERY PLAN)?)? (
		alter_table_stmt
		| analyze_stmt
		| attach_stmt
		| begin_stmt
		| commit_stmt
		| create_index_stmt
		| create_table_stmt
		| create_trigger_stmt
		| create_view_stmt
		| create_virtual_table_stmt
		| delete_stmt
		| delete_stmt_limited
		| detach_stmt
		| drop_stmt
		| insert_stmt
		| pragma_stmt
		| reindex_stmt
		| release_stmt
		| rollback_stmt
		| savepoint_stmt
		| select_stmt
		| update_stmt
		| update_stmt_limited
		| vacuum_stmt
	);

alter_table_stmt:
	ALTER TABLE (schema_name SQL_DOT)? table_name (
		RENAME (
			(TO new_table_name)
			| (
				COLUMN? old_column_name = column_name TO new_column_name = column_name
			)
		)
		| ADD COLUMN? column_def
	);

analyze_stmt:
	ANALYZE (
		schema_name
		| (schema_name SQL_DOT)? table_or_index_name
	)?;

attach_stmt: ATTACH DATABASE? expr AS schema_name;

begin_stmt:
	BEGIN (DEFERRED | IMMEDIATE | EXCLUSIVE)? (
		TRANSACTION transaction_name?
	)?;

commit_stmt: ( COMMIT | END) TRANSACTION?;

rollback_stmt:
	ROLLBACK TRANSACTION? (TO SAVEPOINT? savepoint_name)?;

savepoint_stmt: SAVEPOINT savepoint_name;

release_stmt: RELEASE SAVEPOINT? savepoint_name;

create_index_stmt:
	CREATE UNIQUE? INDEX (IF NOT EXISTS)? (schema_name SQL_DOT)? index_name ON table_name SQL_OPEN_PAR
		indexed_column (SQL_COMMA indexed_column)* SQL_CLOSE_PAR (WHERE expr)?;

indexed_column:
	(column_name | expr) (COLLATE collation_name)? asc_desc?;

create_table_stmt:
	CREATE (TEMP | TEMPORARY)? TABLE (IF NOT EXISTS)? (
		schema_name SQL_DOT
	)? table_name (
		(
			SQL_OPEN_PAR column_def (SQL_COMMA column_def)* (
				SQL_COMMA table_constraint
			)* SQL_CLOSE_PAR (WITHOUT rowID = IDENTIFIER)?
		)
		| (AS select_stmt)
	);

column_def: column_name type_name? column_constraint*;

type_name:
	name+ (
		SQL_OPEN_PAR signed_number SQL_CLOSE_PAR
		| SQL_OPEN_PAR signed_number SQL_COMMA signed_number SQL_CLOSE_PAR
	)?;

column_constraint: (CONSTRAINT name)? (
		(PRIMARY KEY asc_desc? conflict_clause? AUTOINCREMENT?)
		| ((NOT NULL_) | UNIQUE) conflict_clause?
		| CHECK SQL_OPEN_PAR expr SQL_CLOSE_PAR
		| DEFAULT (
			signed_number
			| literal_value
			| (SQL_OPEN_PAR expr SQL_CLOSE_PAR)
		)
		| COLLATE collation_name
		| foreign_key_clause
		| (GENERATED ALWAYS)? AS SQL_OPEN_PAR expr SQL_CLOSE_PAR (STORED | VIRTUAL)?
	);

signed_number: ( SQL_PLUS | SQL_MINUS)? NUMERIC_LITERAL;

table_constraint: (CONSTRAINT name)? (
		(
			(PRIMARY KEY | UNIQUE) SQL_OPEN_PAR indexed_column (
				SQL_COMMA indexed_column
			)* SQL_CLOSE_PAR conflict_clause?
		)
		| (CHECK SQL_OPEN_PAR expr SQL_CLOSE_PAR)
		| (
			FOREIGN KEY SQL_OPEN_PAR column_name (SQL_COMMA column_name)* SQL_CLOSE_PAR foreign_key_clause
		)
	);

foreign_key_clause:
	REFERENCES foreign_table (
		SQL_OPEN_PAR column_name ( SQL_COMMA column_name)* SQL_CLOSE_PAR
	)? (
		(
			ON (DELETE | UPDATE) (
				(SET (NULL_ | DEFAULT))
				| CASCADE
				| RESTRICT
				| (NO ACTION)
			)
		)
		| (MATCH name)
	)* (NOT? DEFERRABLE ( INITIALLY (DEFERRED | IMMEDIATE))?)?;

conflict_clause:
	ON CONFLICT (ROLLBACK | ABORT | FAIL | IGNORE | REPLACE);
create_trigger_stmt:
	CREATE (TEMP | TEMPORARY)? TRIGGER (IF NOT EXISTS)? (
		schema_name SQL_DOT
	)? trigger_name (BEFORE | AFTER | (INSTEAD OF))? (
		DELETE
		| INSERT
		| (UPDATE ( OF column_name ( SQL_COMMA column_name)*)?)
	) ON table_name (FOR EACH ROW)? (WHEN expr)? BEGIN (
		(update_stmt | insert_stmt | delete_stmt | select_stmt) SQL_SEMICOL
	)+ END;

create_view_stmt:
	CREATE (TEMP | TEMPORARY)? VIEW (IF NOT EXISTS)? (
		schema_name SQL_DOT
	)? view_name (SQL_OPEN_PAR column_name (SQL_COMMA column_name)* SQL_CLOSE_PAR)? AS select_stmt;

create_virtual_table_stmt:
	CREATE VIRTUAL TABLE (IF NOT EXISTS)? (schema_name SQL_DOT)? table_name USING module_name (
		SQL_OPEN_PAR module_argument (SQL_COMMA module_argument)* SQL_CLOSE_PAR
	)?;

with_clause:
	WITH RECURSIVE? cte_table_name AS SQL_OPEN_PAR select_stmt SQL_CLOSE_PAR (
		SQL_COMMA cte_table_name AS SQL_OPEN_PAR select_stmt SQL_CLOSE_PAR
	)*;

cte_table_name:
	table_name (SQL_OPEN_PAR column_name ( SQL_COMMA column_name)* SQL_CLOSE_PAR)?;

recursive_cte:
	cte_table_name AS SQL_OPEN_PAR initial_select UNION ALL? recursive_select SQL_CLOSE_PAR;

common_table_expression:
	table_name (SQL_OPEN_PAR column_name ( SQL_COMMA column_name)* SQL_CLOSE_PAR)? AS SQL_OPEN_PAR select_stmt SQL_CLOSE_PAR;

delete_stmt:
	with_clause? DELETE FROM qualified_table_name (WHERE expr)?;

delete_stmt_limited:
	with_clause? DELETE FROM qualified_table_name (WHERE expr)? (
		order_by_stmt? limit_stmt
	)?;

detach_stmt: DETACH DATABASE? schema_name;

drop_stmt:
	DROP object = (INDEX | TABLE | TRIGGER | VIEW) (IF EXISTS)? (
		schema_name SQL_DOT
	)? any_name;

/*
 SQLite understands the following binary operators, in order from highest to lowest precedence: || /
 % + - << >> & | < <= > >= = == != <> IS IS NOT IN LIKE GLOB MATCH REGEXP AND OR
 */
expr:
	literal_value
	| BIND_PARAMETER
	| ( ( schema_name SQL_DOT)? table_name SQL_DOT)? column_name
	| unary_operator expr
	| expr '||' expr
	| expr ( SQL_STAR | SQL_DIV | '%') expr
	| expr ( SQL_PLUS | SQL_MINUS) expr
	| expr ( '<<' | '>>' | SQL_AMP | SQL_PIPE) expr
	| expr ( '<' | '<=' | '>' | '>=') expr
	| expr (
		SQL_ASSIGN
		| '=='
		| '!='
		| '<>'
		| IS
		| IS NOT
		| IN
		| LIKE
		| GLOB
		| MATCH
		| REGEXP
	) expr
	| expr AND expr
	| expr OR expr
	| function_name SQL_OPEN_PAR ((DISTINCT? expr ( SQL_COMMA expr)*) | SQL_STAR)? SQL_CLOSE_PAR filter_clause? over_clause?
	| SQL_OPEN_PAR expr (SQL_COMMA expr)* SQL_CLOSE_PAR
	| CAST SQL_OPEN_PAR expr AS type_name SQL_CLOSE_PAR
	| expr COLLATE collation_name
	| expr NOT? (LIKE | GLOB | REGEXP | MATCH) expr (ESCAPE expr)?
	| expr ( ISNULL | NOTNULL | (NOT NULL_))
	| expr IS NOT? expr
	| expr NOT? BETWEEN expr AND expr
	| expr NOT? IN (
		(SQL_OPEN_PAR (select_stmt | expr ( SQL_COMMA expr)*)? SQL_CLOSE_PAR)
		| (( schema_name SQL_DOT)? table_name)
		| (
			(schema_name SQL_DOT)? table_function_name SQL_OPEN_PAR (
				expr (SQL_COMMA expr)*
			)? SQL_CLOSE_PAR
		)
	)
	| ( ( NOT)? EXISTS)? SQL_OPEN_PAR select_stmt SQL_CLOSE_PAR
	| CASE expr? ( WHEN expr THEN expr)+ ( ELSE expr)? END
	| raise_function;

raise_function:
	RAISE SQL_OPEN_PAR (
		IGNORE
		| (( ROLLBACK | ABORT | FAIL) SQL_COMMA error_message)
	) SQL_CLOSE_PAR;

literal_value:
	NUMERIC_LITERAL
	| STRING_LITERAL
	| BLOB_LITERAL
	| NULL_
	| TRUE_
	| FALSE_
	| CURRENT_TIME
	| CURRENT_DATE
	| CURRENT_TIMESTAMP;

insert_stmt:
	with_clause? (
		INSERT
		| REPLACE
		| (
			INSERT OR (
				REPLACE
				| ROLLBACK
				| ABORT
				| FAIL
				| IGNORE
			)
		)
	) INTO (schema_name SQL_DOT)? table_name (AS table_alias)? (
		SQL_OPEN_PAR column_name ( SQL_COMMA column_name)* SQL_CLOSE_PAR
	)? (
		(
			(
				VALUES SQL_OPEN_PAR expr (SQL_COMMA expr)* SQL_CLOSE_PAR (
					SQL_COMMA SQL_OPEN_PAR expr ( SQL_COMMA expr)* SQL_CLOSE_PAR
				)*
			)
			| select_stmt
		) upsert_clause?
	)
	| (DEFAULT VALUES);

upsert_clause:
	ON CONFLICT (
		SQL_OPEN_PAR indexed_column (SQL_COMMA indexed_column)* SQL_CLOSE_PAR (WHERE expr)?
	)? DO (
		NOTHING
		| (
			UPDATE SET (
				(column_name | column_name_list) SQL_ASSIGN expr (
					SQL_COMMA (column_name | column_name_list) SQL_ASSIGN expr
				)* (WHERE expr)?
			)
		)
	);

pragma_stmt:
	PRAGMA (schema_name SQL_DOT)? pragma_name (
		SQL_ASSIGN pragma_value
		| SQL_OPEN_PAR pragma_value SQL_CLOSE_PAR
	)?;

pragma_value: signed_number | name | STRING_LITERAL;

reindex_stmt:
	REINDEX (
		collation_name
		| (( schema_name SQL_DOT)? ( table_name | index_name))
	)?;

select_stmt:
	common_table_stmt? select_core (
		compound_operator select_core
	)* order_by_stmt? limit_stmt?;

join_clause:
	table_or_subquery (
		join_operator table_or_subquery join_constraint?
	)*;

select_core:
	(
		SELECT (DISTINCT | ALL)? result_column (
			SQL_COMMA result_column
		)* (
			FROM (
				table_or_subquery (SQL_COMMA table_or_subquery)*
				| join_clause
			)
		)? (WHERE expr)? (
			GROUP BY expr (SQL_COMMA expr)* (HAVING expr)?
		)? (
			WINDOW window_name AS window_defn (
				SQL_COMMA window_name AS window_defn
			)*
		)?
	)
	| VALUES SQL_OPEN_PAR expr (SQL_COMMA expr)* SQL_CLOSE_PAR (
		SQL_COMMA SQL_OPEN_PAR expr ( SQL_COMMA expr)* SQL_CLOSE_PAR
	)*;

factored_select_stmt: select_stmt;

simple_select_stmt:
	common_table_stmt? select_core order_by_stmt? limit_stmt?;

compound_select_stmt:
	common_table_stmt? select_core (
		((UNION ALL?) | INTERSECT | EXCEPT) select_core
	)+ order_by_stmt? limit_stmt?;

table_or_subquery: (
		(schema_name SQL_DOT)? table_name (AS? table_alias)? (
			(INDEXED BY index_name)
			| (NOT INDEXED)
		)?
	)
	| (
		(schema_name SQL_DOT)? table_function_name SQL_OPEN_PAR expr (
			SQL_COMMA expr
		)* SQL_CLOSE_PAR (AS? table_alias)?
	)
	| SQL_OPEN_PAR (
		table_or_subquery (SQL_COMMA table_or_subquery)*
		| join_clause
	) SQL_CLOSE_PAR
	| (SQL_OPEN_PAR select_stmt SQL_CLOSE_PAR ( AS? table_alias)?);

result_column:
	SQL_STAR
	| table_name SQL_DOT SQL_STAR
	| expr ( AS? column_alias)?;

join_operator:
	SQL_COMMA
	| (NATURAL? ( (LEFT OUTER?) | INNER | CROSS)? JOIN);

join_constraint:
	(ON expr)
	| (USING SQL_OPEN_PAR column_name ( SQL_COMMA column_name)* SQL_CLOSE_PAR);

compound_operator: (UNION ALL?) | INTERSECT | EXCEPT;

update_stmt:
	with_clause? UPDATE (
		OR (ROLLBACK | ABORT | REPLACE | FAIL | IGNORE)
	)? qualified_table_name SET (column_name | column_name_list) SQL_ASSIGN expr (
		SQL_COMMA (column_name | column_name_list) SQL_ASSIGN expr
	)* (WHERE expr)?;

column_name_list: SQL_OPEN_PAR column_name (SQL_COMMA column_name)* SQL_CLOSE_PAR;

update_stmt_limited:
	with_clause? UPDATE (
		OR (ROLLBACK | ABORT | REPLACE | FAIL | IGNORE)
	)? qualified_table_name SET (column_name | column_name_list) SQL_ASSIGN expr (
		SQL_COMMA (column_name | column_name_list) SQL_ASSIGN expr
	)* (WHERE expr)? (order_by_stmt? limit_stmt)?;

qualified_table_name: (schema_name SQL_DOT)? table_name (AS alias)? (
		(INDEXED BY index_name)
		| (NOT INDEXED)
	)?;

vacuum_stmt: VACUUM schema_name? (INTO filename)?;

filter_clause: FILTER SQL_OPEN_PAR WHERE expr SQL_CLOSE_PAR;

window_defn:
	SQL_OPEN_PAR base_window_name? (PARTITION BY expr (SQL_COMMA expr)*)? (
		ORDER BY ordering_term (SQL_COMMA ordering_term)*
	) frame_spec? SQL_CLOSE_PAR;

over_clause:
	OVER (
		window_name
		| (
			SQL_OPEN_PAR base_window_name? (PARTITION BY expr (SQL_COMMA expr)*)? (
				ORDER BY ordering_term (SQL_COMMA ordering_term)*
			)? frame_spec? SQL_CLOSE_PAR
		)
	);

frame_spec:
	frame_clause (
		EXCLUDE (NO OTHERS)
		| (CURRENT ROW)
		| GROUP
		| TIES
	)?;

frame_clause: (RANGE | ROWS | GROUPS) (
		frame_single
		| BETWEEN frame_left AND frame_right
	);
simple_function_invocation:
	simple_func SQL_OPEN_PAR ((expr (SQL_COMMA expr)*) | SQL_STAR) SQL_CLOSE_PAR;

aggregate_function_invocation:
	aggregate_func SQL_OPEN_PAR ((DISTINCT? expr (SQL_COMMA expr)*) | SQL_STAR)? SQL_CLOSE_PAR filter_clause?;

window_function_invocation:
	window_function SQL_OPEN_PAR ((expr (SQL_COMMA expr)*) | SQL_STAR)? SQL_CLOSE_PAR filter_clause? OVER (
		window_defn
		| window_name
	);

common_table_stmt: //additional structures
	WITH RECURSIVE? common_table_expression (
		SQL_COMMA common_table_expression
	)*;

order_by_stmt: ORDER BY ordering_term ( SQL_COMMA ordering_term)*;

limit_stmt: LIMIT expr ( (OFFSET | SQL_COMMA) expr)?;

ordering_term:
	expr (COLLATE collation_name)? asc_desc? (
		NULLS (FIRST | LAST)
	)?;
asc_desc: ASC | DESC;

frame_left:
	(expr PRECEDING)
	| (expr FOLLOWING)
	| (CURRENT ROW)
	| (UNBOUNDED PRECEDING);

frame_right:
	(expr PRECEDING)
	| (expr FOLLOWING)
	| (CURRENT ROW)
	| (UNBOUNDED FOLLOWING);

frame_single:
	(expr PRECEDING)
	| (UNBOUNDED PRECEDING)
	| (CURRENT ROW);

// unknown

window_function:
	(FIRST_VALUE | LAST_VALUE) SQL_OPEN_PAR expr SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr_asc_desc
		frame_clause? SQL_CLOSE_PAR
	| (CUME_DIST | PERCENT_RANK) SQL_OPEN_PAR SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr? SQL_CLOSE_PAR
	| (DENSE_RANK | RANK | ROW_NUMBER) SQL_OPEN_PAR SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr_asc_desc SQL_CLOSE_PAR
	| (LAG | LEAD) SQL_OPEN_PAR expr offset? default_value? SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr_asc_desc
		SQL_CLOSE_PAR
	| NTH_VALUE SQL_OPEN_PAR expr SQL_COMMA signed_number SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr_asc_desc
		frame_clause? SQL_CLOSE_PAR
	| NTILE SQL_OPEN_PAR expr SQL_CLOSE_PAR OVER SQL_OPEN_PAR partition_by? order_by_expr_asc_desc SQL_CLOSE_PAR;

offset: SQL_COMMA signed_number;

default_value: SQL_COMMA signed_number;

partition_by: PARTITION BY expr+;

order_by_expr: ORDER BY expr+;

order_by_expr_asc_desc: ORDER BY order_by_expr_asc_desc;

expr_asc_desc: expr asc_desc? (SQL_COMMA expr asc_desc?)*;

//TODO BOTH OF THESE HAVE TO BE REWORKED TO FOLLOW THE SPEC
initial_select: select_stmt;

recursive_select: select_stmt;

unary_operator: SQL_MINUS | SQL_PLUS | '~' | NOT;

error_message: STRING_LITERAL;

module_argument: // TODO check what exactly is permitted here
	expr
	| column_def;

column_alias: IDENTIFIER | STRING_LITERAL;

keyword:
	ABORT
	| ACTION
	| ADD
	| AFTER
	| ALL
	| ALTER
	| ANALYZE
	| AND
	| AS
	| ASC
	| ATTACH
	| AUTOINCREMENT
	| BEFORE
	| BEGIN
	| BETWEEN
	| BY
	| CASCADE
	| CASE
	| CAST
	| CHECK
	| COLLATE
	| COLUMN
	| COMMIT
	| CONFLICT
	| CONSTRAINT
	| CREATE
	| CROSS
	| CURRENT_DATE
	| CURRENT_TIME
	| CURRENT_TIMESTAMP
	| DATABASE
	| DEFAULT
	| DEFERRABLE
	| DEFERRED
	| DELETE
	| DESC
	| DETACH
	| DISTINCT
	| DROP
	| EACH
	| ELSE
	| END
	| ESCAPE
	| EXCEPT
	| EXCLUSIVE
	| EXISTS
	| EXPLAIN
	| FAIL
	| FOR
	| FOREIGN
	| FROM
	| FULL
	| GLOB
	| GROUP
	| HAVING
	| IF
	| IGNORE
	| IMMEDIATE
	| IN
	| INDEX
	| INDEXED
	| INITIALLY
	| INNER
	| INSERT
	| INSTEAD
	| INTERSECT
	| INTO
	| IS
	| ISNULL
	| JOIN
	| KEY
	| LEFT
	| LIKE
	| LIMIT
	| MATCH
	| NATURAL
	| NO
	| NOT
	| NOTNULL
	| NULL_
	| OF
	| OFFSET
	| ON
	| OR
	| ORDER
	| OUTER
	| PLAN
	| PRAGMA
	| PRIMARY
	| QUERY
	| RAISE
	| RECURSIVE
	| REFERENCES
	| REGEXP
	| REINDEX
	| RELEASE
	| RENAME
	| REPLACE
	| RESTRICT
	| RIGHT
	| ROLLBACK
	| ROW
	| ROWS
	| SAVEPOINT
	| SELECT
	| SET
	| TABLE
	| TEMP
	| TEMPORARY
	| THEN
	| TO
	| TRANSACTION
	| TRIGGER
	| UNION
	| UNIQUE
	| UPDATE
	| USING
	| VACUUM
	| VALUES
	| VIEW
	| VIRTUAL
	| WHEN
	| WHERE
	| WITH
	| WITHOUT
	| FIRST_VALUE
	| OVER
	| PARTITION
	| RANGE
	| PRECEDING
	| UNBOUNDED
	| CURRENT
	| FOLLOWING
	| CUME_DIST
	| DENSE_RANK
	| LAG
	| LAST_VALUE
	| LEAD
	| NTH_VALUE
	| NTILE
	| PERCENT_RANK
	| RANK
	| ROW_NUMBER
	| GENERATED
	| ALWAYS
	| STORED
	| TRUE_
	| FALSE_
	| WINDOW
	| NULLS
	| FIRST
	| LAST
	| FILTER
	| GROUPS
	| EXCLUDE;

// TODO check all names below

name: any_name;

function_name: any_name;

schema_name: any_name;

table_name: any_name;

table_or_index_name: any_name;

new_table_name: any_name;

column_name: any_name;

collation_name: any_name;

foreign_table: any_name;

index_name: any_name;

trigger_name: any_name;

view_name: any_name;

module_name: any_name;

pragma_name: any_name;

savepoint_name: any_name;

table_alias: any_name;

transaction_name: any_name;

window_name: any_name;

alias: any_name;

filename: any_name;

base_window_name: any_name;

simple_func: any_name;

aggregate_func: any_name;

table_function_name: any_name;

any_name:
	IDENTIFIER
	| keyword
	| STRING_LITERAL
	| SQL_OPEN_PAR any_name SQL_CLOSE_PAR;
