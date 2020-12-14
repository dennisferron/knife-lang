lexer grammar Subst;

// ----------------- Default mode, everything OUTSIDE of substitution macro ---------------
OPEN        :   '${' -> pushMode(INSIDE) ;
RawText     :   ~'$'+ ; // match any char except $

// ----------------- Everything INSIDE of a substitution macro ---------------------
mode INSIDE;

CLOSE       :   '}' -> popMode ; // back to default mode
Identifier    :   ALPHA (ALPHA|DIGIT)* ;
WS           :   [ \t\r\n] -> skip ;

fragment
ALPHA       :   [a-zA-Z_] ;

fragment
DIGIT       :   [0-9] ;
