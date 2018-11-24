#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../database/dbms.h"
#include "../table/table_header.h"

void fill_table_header(table_header_t *header, const table_def_t *table);
void execute_create_table(const table_def_t *table)
{
	table_header_t *header = new table_header_t;
	fill_table_header(header, table);
	dbms::get_instance()->create_table(header);
	delete header;
	// TODO: free memory of table
}

void execute_create_database(const char *db_name)
{
	dbms::get_instance()->create_database(db_name);
	free((char*)db_name);
}

void execute_use_database(const char *db_name)
{
	dbms::get_instance()->switch_database(db_name);
	free((char*)db_name);
}

void execute_drop_database(const char *db_name)
{
	dbms::get_instance()->drop_database(db_name);
	free((char*)db_name);
}

void execute_show_database(const char *db_name)
{
	printf("[show] database name = %s\n", db_name);
}

void execute_drop_table(const char *table_name)
{
	printf("[drop] table name = %s\n", table_name);
}

void execute_show_table(const char *table_name)
{
	printf("[show] table name = %s\n", table_name);
}

void execute_insert(const insert_info_t *insert_info)
{
	dbms::get_instance()->insert_rows(insert_info);
	// TODO: free memory
}

void execute_delete(const delete_info_t *delete_info)
{
	dbms::get_instance()->delete_rows(delete_info);
}

void execute_update(const update_info_t *update_info)
{
	dbms::get_instance()->update_rows(update_info);
}

void execute_create_index(const char *table_name, const char *col_name)
{
	dbms::get_instance()->create_index(table_name, col_name);
	free((char*)table_name);
	free((char*)col_name);
}

void execute_drop_index(const char *table_name, const char *col_name)
{
	dbms::get_instance()->drop_index(table_name, col_name);
	free((char*)table_name);
	free((char*)col_name);
}

void traverse_expr(const expr_node_t *expr_node)
{
	if(expr_node == nullptr) return;
	if(expr_node->term_type != TERM_NONE)
	{
		switch(expr_node->term_type)
		{
			case TERM_INT: printf("[int: %d]", expr_node->val_i); break;
			case TERM_FLOAT: printf("[float: %f]", expr_node->val_f); break;
			case TERM_STRING: printf("[str: %s]", expr_node->val_s); break;
			default: break;
		}
	} else {
	}
}

void execute_quit()
{
	dbms::get_instance()->close_database();
	printf("[exit] good bye!\n");
}


