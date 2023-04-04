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

let_stmt: LET var_name=IDENT (COLON var_type=type_annotation)? ASGN var_init=expr;
fresh_stmt: FRESH var_name=IDENT (COLON var_type=type_annotation)?;
yield_stmt: YIELD expr;
member_stmt: MEMBER member_name=identifier
    ASGN member_type=identifier
        OPEN_PAR call_param_list CLOSE_PAR;

type_annotation: IDENT;

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
call_param_list: expr (COMMA expr)*;

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
