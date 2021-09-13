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

relation: relation_header OPEN_BRACE stat* CLOSE_BRACE;

relation_header: RELATION relation_name OPEN_PAR relation_param_list? CLOSE_PAR;
relation_name: IDENT;
relation_param_list: relation_param (COMMA relation_param)*;
relation_param: IDENT (COLON IDENT)?;

stat: (
        let_stmt
    |   fresh_stmt
    |   yield_stmt
    |   member_stmt
    |   expr
    |   IDENT ASGN expr
    ) SEMICOLON
    ;

let_stmt: LET IDENT type_annotation? ASGN expr;
fresh_stmt: FRESH IDENT type_annotation?;
yield_stmt: YIELD expr;
member_stmt: MEMBER member_name=identifier
    ASGN member_type=identifier
        OPEN_PAR call_param_list CLOSE_PAR;

type_annotation: COLON IDENT;

identifier: IDENT;

expr:
	    OPEN_QUASIQUOTE quasiquote #quasiquoteExpr
    |   lhs=expr op=(MULT|DIV|AMP) rhs=expr #binOpExpr1
    |   lhs=expr op=(PLUS|MINUS|BAR) rhs=expr #binOpExpr2
    |   call_expression #callExpr
    |   INT  #intExpr
    |   identifier #idExpr
    |   OPEN_PAR expr CLOSE_PAR #parenExpr
    |   lhs=identifier op=DOT rhs=identifier #dotExpr
    ;

call_expression: call_target_name=IDENT OPEN_PAR call_param_list CLOSE_PAR;
call_param_list: call_param (COMMA call_param)*;
call_param: expr;

quasiquote:
	   QUASIQUOTE_SQL SQL_ANYTHING* SQL_CLOSE_QUASIQUOTE #quasiQuoteSql
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
