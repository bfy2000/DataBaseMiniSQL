%{
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void yyerror(char *s, ...);
void emit(char *s, ...);
%}

%union {
	int intval;
	double floatval;
	char *strval;
	int subtok;
}

%token <strval> NAME
%token <strval> STRING
%token <intval> INTNUM
%token <intval> BOOL
%token <floatval> APPROXNUM


%token INSERT
%token INTO
%token VALUES

%type <intval> insert_vals
%type <intval> insert_vals_list

%start stmt_list

%%
stmt_list: stmt ';'
  | stmt_list stmt ';'
  ;

stmt: insert_stmt { emit("STMT"); }
    ;

insert_stmt: INSERT INTO NAME VALUES insert_vals_list
    { emit("INSERTVALS %d %s", $5, $3); free($3); }
    ;

insert_vals_list: '(' insert_vals ')' { emit("VALUES %d", $2); $$ = 1; }
    | insert_vals_list ',' '(' insert_vals ')' { emit("VALUES %d", $4); $$ = $1 + 1; }

insert_vals: expr { $$ = 1; }
    | insert_vals ',' expr { $$ = $1 + 1; }
    ;

expr: NAME          { emit("NAME %s", $1); free($1); }
    | NAME '.' NAME { emit("FIELDNAME %s.%s", $1, $3); free($1); free($3); }
    | STRING        { emit("STRING %s", $1); free($1); }
    | INTNUM        { emit("NUMBER %d", $1); }
    | APPROXNUM     { emit("FLOAT %g", $1); }
    | BOOL          { emit("BOOL %d", $1); }
    ;


%%

void
emit(char *s, ...)
{
  extern yylineno;

  va_list ap;
  va_start(ap, s);

  printf("rpn: ");
  vfprintf(stdout, s, ap);
  printf("\n");
}


void
yyerror(char *s, ...)
{
  extern yylineno;

  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

main(int ac, char **av)
{
  extern FILE *yyin;

  if(ac > 1 && !strcmp(av[1], "-d")) {
    yydebug = 1; ac--; av++;
  }

  if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
    perror(av[1]);
    exit(1);
  }

  if(!yyparse())
    printf("SQL parse worked\n");
  else
    printf("SQL parse failed\n");
} /* main */



