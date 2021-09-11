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
parser grammar KnifeParser;

options {
	tokenVocab = KnifeLexer;
}

/** The start rule; begin parsing here. */
prog:   relation* EOF ;

relation: relation_header KNIFE_OPEN_BRACE stat* KNIFE_CLOSE_BRACE;

relation_header: KNIFE_RELATION relation_name KNIFE_OPEN_PAR relation_param_list? KNIFE_CLOSE_PAR;
relation_name: KNIFE_ID;
relation_param_list: relation_param (KNIFE_COMMA relation_param)*;
relation_param: KNIFE_ID (KNIFE_COLON KNIFE_ID)?;

stat: (
        let_stmt
    |   fresh_stmt
    |   yield_stmt
    |   member_stmt
    |   expr
    |   KNIFE_ID KNIFE_ASGN expr
    ) KNIFE_SEMICOLON
    ;

let_stmt: KNIFE_LET KNIFE_ID type_annotation? KNIFE_ASGN expr;
fresh_stmt: KNIFE_FRESH KNIFE_ID type_annotation?;
yield_stmt: KNIFE_YIELD expr;
member_stmt: KNIFE_MEMBER member_name=identifier
    KNIFE_ASGN member_type=identifier
        KNIFE_OPEN_PAR call_param_list KNIFE_CLOSE_PAR;

type_annotation: KNIFE_COLON KNIFE_ID;

identifier: KNIFE_ID;

expr:
	    KNIFE_OPEN_QUASIQUOTE quasiquote #quasiquoteExpr
    |   expr op=(KNIFE_MULT|KNIFE_DIV|KNIFE_AMP) expr #binOpExpr1
    |   expr op=(KNIFE_PLUS|KNIFE_MINUS|KNIFE_BAR) expr #binOpExpr2
    |   call_expression #callExpr
    |   KNIFE_INT  #intExpr
    |   identifier #idExpr
    |   KNIFE_OPEN_PAR expr KNIFE_CLOSE_PAR #parenExpr
    |   identifier op=KNIFE_DOT identifier #dotExpr
    ;

call_expression: call_target_name=KNIFE_ID KNIFE_OPEN_PAR call_param_list KNIFE_CLOSE_PAR;
call_param_list: call_param (KNIFE_COMMA call_param)*;
call_param: expr;

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
