%option noyywrap nodefault yylineno case-insensitive
%{
#include "sql.tab.h"
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);

int oldstate;

%}

%x COMMENT

%%

AND     { return AND; }
CHAR    { return CHAR; }
CREATE  { return CREATE; }
DOUBLE  { return DOUBLE; }
DROP    { return DROP; }
EXECFILE    { return EXECFILE; }
FLOAT   { return FLOAT; }
FROM    { return FROM; }
INDEX   { return INDEX; }
INSERT	{ return INSERT; }
INT     { return INT; }
INTEGER { return INTEGER; }
INTO	{ return INTO; }
KEY     { return KEY; }
ON      { return ON; }
PRIMARY { return PRIMARY; }
QUIT    { return QUIT; }
SELECT  { return SELECT; }
TABLE   { return TABLE; }
UNIQUE  { return UNIQUE; }
VALUES	{ return VALUES; }
WHERE   { return WHERE; }


-?[0-9]+	        { yylval.intval = atoi(yytext); return INTNUM; } 
-?[0-9]+"."[0-9]* |
-?"."[0-9]+	|
-?[0-9]+E[-+]?[0-9]+	|
-?[0-9]+"."[0-9]*E[-+]?[0-9]+ |
-?"."[0-9]*E[-+]?[0-9]+	{ yylval.floatval = atof(yytext) ;
                                  return APPROXNUM; }


'(\\.|''|[^'\n])*'	|
\"(\\.|\"\"|[^"\n])*\"  { yylval.strval = strdup(yytext); return STRING; }
'(\\.|[^'\n])*$	        { yyerror("Unterminated string %s", yytext); }
\"(\\.|[^"\n])*$	{ yyerror("Unterminated string %s", yytext); }

X'[0-9A-F]+' |  
0X[0-9A-F]+  { yylval.strval = strdup(yytext); return STRING; }

0B[01]+      |
B'[01]+'     { yylval.strval = strdup(yytext); return STRING; }


[-+&~|^/%*(),.;!]   { return yytext[0]; }

"&&"		{ return AND; }
"="	{ yylval.subtok = 4; return COMPARISON; }
">="	{ yylval.subtok = 6; return COMPARISON; }
">"	{ yylval.subtok = 2; return COMPARISON; }
"<="	{ yylval.subtok = 5; return COMPARISON; }
"<"	{ yylval.subtok = 1; return COMPARISON; }
"!="	|
"<>"	{ yylval.subtok = 3; return COMPARISON; }


[A-Za-z][A-Za-z0-9_]*	{ yylval.strval = strdup(yytext);
                          return NAME; }
`[^`/\\.\n]+`           { yylval.strval = strdup(yytext+1);
                          yylval.strval[yyleng-2] = 0;
                          return NAME; }
`[^`\n]*$               { yyerror("unterminated quoted name %s", yytext); }



#.*		;
"--"[ \t].*	;

"/*"            { oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>"*/"   { BEGIN oldstate; }
<COMMENT>.|\n   ;
<COMMENT><<EOF>> { yyerror("unclosed comment"); }


[ \t\n]         
.               { yyerror("mystery character '%c'", *yytext); }

%%