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
    |   member_stmt
    |   expr_expr
    |   EXPR_ID EXPR_ASGN expr_expr
    ) EXPR_SEMICOLON
    ;

let_stmt: EXPR_LET EXPR_ID type_annotation? EXPR_ASGN expr_expr;
fresh_stmt: EXPR_FRESH EXPR_ID type_annotation?;
yield_stmt: EXPR_YIELD expr_expr;
member_stmt: EXPR_MEMBER member_name=expr_identifier
    EXPR_ASGN member_type=expr_identifier
        EXPR_OPEN_PAR call_param_list EXPR_CLOSE_PAR;

type_annotation: EXPR_COLON EXPR_ID;

expr_identifier: EXPR_ID;

expr_expr:
	    EXPR_OPEN_QUASIQUOTE quasiquote #quasiquoteExpr
    |   expr_expr op=(EXPR_MULT|EXPR_DIV|EXPR_AMP) expr_expr #binOpExpr1
    |   expr_expr op=(EXPR_PLUS|EXPR_MINUS|EXPR_BAR) expr_expr #binOpExpr2
    |   call_expression #callExpr
    |   EXPR_INT  #intExpr
    |   expr_identifier #idExpr
    |   EXPR_OPEN_PAR expr_expr EXPR_CLOSE_PAR #parenExpr
    |   expr_identifier op=EXPR_DOT expr_identifier #dotExpr
    ;

call_expression: call_target_name=EXPR_ID EXPR_OPEN_PAR call_param_list EXPR_CLOSE_PAR;
call_param_list: call_param (EXPR_COMMA call_param)*;
call_param: expr_expr;

quasiquote:
	   QUASIQUOTE_SQL SQL_ANYTHING SQL_CLOSE_QUASIQUOTE #quasiQuoteSql
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
