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
%left <subtok> COMPARISON /* = <> < > <= >= <=> */

%token AND
%token CHAR
%token CREATE
%token DOUBLE
%token DROP
%token EXECFILE
%token FLOAT
%token FROM
%token INDEX
%token INSERT
%token INT
%token INTEGER
%token INTO
%token KEY
%token ON
%token PRIMARY
%token QUIT
%token SELECT
%token TABLE
%token UNIQUE
%token VALUES
%token WHERE

%type <intval> insert_vals
%type <intval> insert_vals_list
%type <intval> create_col_list
%type <intval> column_list
%type <intval> column_atts
%type <intval> data_type
%type <intval> opt_length
%type <intval> select_expr_list


%start stmt_list

%%
stmt_list: stmt ';'
  | stmt_list stmt ';'
  ;

/** insert **/
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



/** drop table **/
stmt: drop_table_stmt { emit("STMT"); }
    ;

drop_table_stmt: DROP TABLE NAME
    { emit("DROPTABLE %s", $3); free($3); }
    ;

/** drop index **/
stmt: drop_index_stmt { emit("STMT"); }
    ;

drop_index_stmt: DROP INDEX NAME
    { emit("DROPINDEX %s", $3); free($3); }
    ;

/** quit **/
stmt: quit_stmt { emit("STMT"); }
    ;

quit_stmt: QUIT
    { emit("QUIT"); }
    ;

/** execfile **/
stmt: execfile_stmt { emit("STMT"); }
    ;

execfile_stmt: EXECFILE STRING
    { emit("EXECFILE %s", $2); free($2); }
    ;

/** create index **/
stmt: create_index_stmt { emit("STMT"); }
    ;

create_index_stmt: CREATE INDEX NAME ON expr_index_attr 
    { emit("CREATEINDEX %s", $3); free($3); }
    ;

/** create table **/
stmt: create_table_stmt { emit("STMT"); }
    ;

create_table_stmt: CREATE TABLE NAME '(' create_col_list ')'
    { emit("CREATETABLE %d %s", $5, $3); free($3); }
    ;

create_col_list: create_definition { $$ = 1; }
    | create_col_list ',' create_definition { $$ = $1 + 1; }
    ;

create_definition: { emit("STARTCOL"); } 
    NAME data_type column_atts
    { emit("COLUMNDEF %d %s", $3, $2); free($2); }
    | PRIMARY KEY '(' column_list ')'    { emit("PRIKEY %d", $4); }
    ;

column_list: NAME { emit("COLUMN %s", $1); free($1); $$ = 1; }
    | column_list ',' NAME  { emit("COLUMN %s", $3); free($3); $$ = $1 + 1; }
    ;

column_atts: { $$ = 0; }
    | column_atts UNIQUE{ emit("ATTR UNIQUE"); $$ = $1 + 1; }
    | column_atts PRIMARY KEY { emit("ATTR PRIKEY"); $$ = $1 + 1; }
    ;

data_type: 
    INT { $$ = 40000; }
    | INTEGER { $$ = 40000; }
    | FLOAT { $$ = 90000; }
    | DOUBLE { $$ = 90000; }
    | CHAR opt_length { $$ = 120000 + $2; }

opt_length: { $$ = 0; }
   | '(' INTNUM ')' { $$ = $2; }
   ;

/** select **/

stmt: select_stmt { emit("STMT"); }
    ;

select_stmt: SELECT select_expr_list FROM table_factor opt_where 
    { emit("SELECTTABLE %d", $2); }
    ;

select_expr_list: NAME { emit("NAME %s", $1); free($1); $$ = 1; }
    | select_expr_list ',' NAME { emit("NAME %s", $3); free($3); $$ = $1 + 1; }
    | '*' { emit("SELECTALL"); $$ = 1; }
    ;

table_factor: NAME { emit("TABLE %s", $1); free($1); }
    ;

opt_where: 
   | WHERE expr { emit("WHERE"); };


/** expression **/
expr: NAME          { emit("NAME %s", $1); free($1); }
    | NAME '.' NAME { emit("FIELDNAME %s.%s", $1, $3); free($1); free($3); }
    | STRING        { emit("STRING %s", $1); free($1); }
    | INTNUM        { emit("NUMBER %d", $1); }
    | APPROXNUM     { emit("FLOAT %g", $1); }
    | BOOL          { emit("BOOL %d", $1); }
    ;

expr_index_attr: NAME '(' NAME ')' 
    { emit("INDEXATTR %s %s", $1, $3); free($1); free($3); }
    ;

expr: expr AND expr { emit("AND"); }
   | expr COMPARISON expr { emit("CMP %d", $2); }
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



