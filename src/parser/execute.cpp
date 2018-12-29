#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../database/dbms.h"
#include "../table/table_header.h"

template<typename T, typename DataDeleter>
void free_linked_list(linked_list_t *linked_list, DataDeleter data_deleter)
{
	for(linked_list_t *l_ptr = linked_list; l_ptr; )
	{
		T* data = (T*)l_ptr->data;
		data_deleter(data);
		linked_list_t *tmp = l_ptr;
		l_ptr = l_ptr->next;
		free(tmp);
	}
}

void free_column_ref(column_ref_t *cref)
{
	free(cref->column);
	free(cref->table);
}

void free_expression(expr_node_t *expr)
{
	if(expr->op == OPERATOR_NONE)
	{
		// terminator
		if(expr->term_type == TERM_STRING)
			free(expr->val_s);
		else if(expr->term_type == TERM_COLUMN_REF)
			free(expr->column_ref);
	} else {
		// non-terminator
		free_expression(expr->left);
		if(expr->right) free_expression(expr->right);
	}

	free(expr);
}

bool fill_table_header(table_header_t *header, const table_def_t *table);
void execute_create_table(const table_def_t *table)
{
	table_header_t *header = new table_header_t;
	if(fill_table_header(header, table))
		dbms::get_instance()->create_table(header);
	else std::fprintf(stderr, "[Error] Fail to create table!\n");
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
	dbms::get_instance()->show_table(table_name);
	free((void*)table_name);
}

void execute_insert(const insert_info_t *insert_info)
{
	dbms::get_instance()->insert_rows(insert_info);
	free(insert_info->table);
	free_linked_list<column_ref_t>(insert_info->columns, free_column_ref);
	free_linked_list<linked_list_t>(insert_info->values, [](linked_list_t *expr_list) {
		free_linked_list<expr_node_t>(expr_list, free_expression);
	} );
	free((void*)insert_info);
}

void execute_delete(const delete_info_t *delete_info)
{
	dbms::get_instance()->delete_rows(delete_info);
}

void execute_select(const select_info_t *select_info)
{
	dbms::get_instance()->select_rows(select_info);
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


