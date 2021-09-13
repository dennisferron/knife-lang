/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2020 by Martin Mirchev
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
 * Developed by : Bart Kiers, bart@big-o.nl
 */
lexer grammar KnifeLexer;

OPEN_QUASIQUOTE : '{|' -> mode(QUASIQUOTE);

LINE_COMMENT: '//' ~[\r\n]* (('\r'? '\n') | EOF) -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;

LET : 'let';
FRESH : 'fresh';
RELATION : 'relation';
YIELD : 'yield';
MEMBER : 'member';

PLUS : '+';
MINUS : '-';
MULT : '*';
DIV : '/';
ASGN : '=';
IDENT  :   [a-zA-Z_] [a-zA-Z_0-9]* ;
INT :   [0-9]+ ;         // match integers
OPEN_PAR : '(';
CLOSE_PAR : ')';
OPEN_BRACE : '{';
CLOSE_BRACE : '}';
COLON : ':';
SEMICOLON : ';';
BAR : '|';
AMP : '&';
COMMA: ',';
DOT: '.';

WS  :   [ \t\r\n]+ -> skip ; // toss out whitespace

mode QUASIQUOTE;
QUASIQUOTE_CSV : 'CSV' -> mode(CSV_HEADER);
QUASIQUOTE_SQL : 'SQL' -> mode(SQLITE);
QUASIQUOTE_WS  :   [ \t]+ -> skip ; // toss out whitespace

mode CSV_HEADER;

CSV_HDR_OPEN_PAR : '(';
CSV_HDR_CLOSE_PAR : ')';
CSV_HDR_COLON : ':';
CSV_HDR_COMMA : ',';
CSV_HDR_ID  :   [a-zA-Z_] [a-zA-Z_0-9]* ;
CSV_HDR_NEWLINE : '\r'? '\n' -> mode(CSV);
CSV_HDR_WS  :   [ \t]+ -> skip ; // toss out whitespace

mode CSV;

CSV_CLOSE_QUASIQUOTE : '|}' -> mode(DEFAULT_MODE);

// TODO: Should CSV_TEXT be below CSV_WS and/or newline?

CSV_COMMA : ',';
CSV_TEXT   : ~[ ,\n\r"|}]+ ;
CSV_NEWLINE : '\r'? '\n';
CSV_STRING : '"' ('""'|~'"')* '"' ; // quote-quote is an escaped quote
CSV_WS  :   [ \t]+ -> skip ; // toss out whitespace

mode SQLITE;

SQL_CLOSE_QUASIQUOTE : '|}' -> mode(DEFAULT_MODE);
SQL_WS  :   [ \t\n]+ -> skip ; // toss out whitespace
SQL_ANYTHING : .+?;
