/* Modified from https://raw.githubusercontent.com/thinkpad20/sql/master/src/yacc/sql.y */

%define parse.error verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

void yyerror(const char *s);

#include "sql.yy.c"

%}

%union {
	char *val_s;
	int   val_i;
	float val_f;
	struct field_item_t       *field_items;
	struct table_def_t        *table_def;
	struct column_ref_t       *column_ref;
	struct linked_list_t      *list;
	struct table_constraint_t *constraint;
}

%token TRUE FALSE NULL_TOKEN MIN MAX SUM AVG COUNT
%token LIKE IS OR AND NOT NEQ GEQ LEQ
%token INTEGER DOUBLE FLOAT CHAR VARCHAR
%token INTO FROM WHERE VALUES JOIN INNER OUTER
%token LEFT RIGHT FULL ASC DESC ORDER BY IN ON AS
%token DISTINCT GROUP USING INDEX TABLE DATABASE
%token DEFAULT UNIQUE PRIMARY FOREIGN REFERENCES CHECK KEY
%token USE CREATE DROP SELECT INSERT UPDATE DELETE SHOW SET

%token IDENTIFIER
%token STRING_LITERAL
%token FLOAT_LITERAL
%token INT_LITERAL

%type <val_s> IDENTIFIER STRING_LITERAL
%type <val_f> FLOAT_LITERAL
%type <val_i> INT_LITERAL

%type <val_i> field_type field_width field_flag field_flags
%type <val_s> table_name

%type <field_items> table_field table_fields
%type <table_def> create_table_stmt
%type <column_ref> column_ref
%type <constraint> table_extra_option
%type <list> column_list table_extra_options table_extra_option_list

%start sql_stmts

%%

sql_stmts  :  sql_stmt
		   |  sql_stmts sql_stmt
		   ;

sql_stmt   :  create_table_stmt
		   ;

create_table_stmt : CREATE TABLE table_name '(' table_fields table_extra_options ')' {
				  	$$ = (table_def_t*)malloc(sizeof(table_def_t));
					$$->name = $3;
					$$->fields = $5;
					$$->constraints = $6;
				  }
				  ;

table_extra_options : ',' table_extra_option_list  { $$ = $2; }
					| /* empty */                  { $$ = NULL; }
					;

table_extra_option_list : table_extra_option_list ',' table_extra_option {
							$$ = (linked_list_t*)malloc(sizeof(linked_list_t));
							$$->data = $3;
							$$->next = $1;
						}
						| table_extra_option {
							$$ = (linked_list_t*)malloc(sizeof(linked_list_t));
							$$->data = $1;
							$$->next = NULL;
						}
						;

table_extra_option : PRIMARY KEY '(' column_list ')' {
				   	$$ = (table_constraint_t*)malloc(sizeof(table_constraint_t));
					$$->type = TABLE_CONSTRAINT_PRIMARY_KEY;
					$$->values = $4;
				   }
				   ;

column_ref   : IDENTIFIER {
			 	$$ = (column_ref_t*)malloc(sizeof(column_ref_t));
				$$->table  = NULL;
				$$->column = $1;
			 }
			 | table_name '.' IDENTIFIER {
			 	$$ = (column_ref_t*)malloc(sizeof(column_ref_t));
				$$->table  = $1;
				$$->column = $3;
			 }
			 ;

column_list  : column_list ',' column_ref {
				linked_list_t *f;
			 	$$ = $1;
				for(f = $1; f->next; f = f->next);
				f->next = (linked_list_t*)malloc(sizeof(linked_list_t));
				f->next->data = $3;
				f->next->next = NULL;
			 }
			 | column_ref {
			 	$$ = (linked_list_t*)malloc(sizeof(linked_list_t));
				$$->data = $1;
				$$->next = NULL;
			 }
			 ;


table_fields : table_field { $$ = $1; }
			 | table_fields ',' table_field {
				field_item_t *f;
			 	$$ = $1;
				for(f = $1; f->next; f = f->next);
				f->next = $3;
			 }
			 ;

table_field  : IDENTIFIER field_type field_width field_flags {
			 	$$ = (field_item_t*)malloc(sizeof(field_item_t));
				$$->name = $1;
				$$->type = $2;
				$$->width = $3;
				$$->flags = $4;
				$$->next = NULL;
			 }
			 ;

field_flags : field_flags field_flag  { $$ = $1 | $2; }
			| /* empty */             { $$ = 0; }
			;

field_flag  : NOT NULL_TOKEN  { $$ = FIELD_FLAG_NOTNULL; }
			;

field_width : '(' INT_LITERAL ')'  { $$ = $2; }
			| /* empty */          { $$ = 0; }
			;

field_type  : INTEGER { $$ = FIELD_TYPE_INT; }
		    | FLOAT   { $$ = FIELD_TYPE_FLOAT; }
		    | DOUBLE  { $$ = FIELD_TYPE_FLOAT; }
		    | CHAR    { $$ = FIELD_TYPE_CHAR; }
		    | VARCHAR { $$ = FIELD_TYPE_VARCHAR; }
		    ;

table_name : IDENTIFIER          { $$ = $1; }
		   | '`' IDENTIFIER '`'  { $$ = $2; }
		   ;

%%

