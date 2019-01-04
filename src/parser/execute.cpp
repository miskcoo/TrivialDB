#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../database/dbms.h"
#include "../table/table_header.h"
#include "../expression/expression.h"

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

void expression::free_exprnode(expr_node_t *expr)
{
	if(!expr) return;
	if(expr->op == OPERATOR_NONE)
	{
		switch(expr->term_type)
		{
			case TERM_STRING:
				free(expr->val_s);
				break;
			case TERM_COLUMN_REF:
				free(expr->column_ref->table);
				free(expr->column_ref->column);
				free(expr->column_ref);
				break;
			case TERM_LITERAL_LIST:
				free_linked_list<expr_node_t>(
					expr->literal_list,
					expression::free_exprnode
				);
				break;
			default:
				break;
		}
	} else {
		free_exprnode(expr->left);
		free_exprnode(expr->right);
	}

	free(expr);
}

void free_column_ref(column_ref_t *cref)
{
	if(!cref) return;
	free(cref->column);
	free(cref->table);
	free(cref);
}

bool fill_table_header(table_header_t *header, const table_def_t *table);

void execute_switch_output(const char *output_filename)
{
	dbms::get_instance()->switch_select_output(output_filename);
	free((void*)output_filename);
}

void execute_create_table(const table_def_t *table)
{
	table_header_t *header = new table_header_t;
	if(fill_table_header(header, table))
		dbms::get_instance()->create_table(header);
	else std::fprintf(stderr, "[Error] Fail to create table!\n");
	delete header;

	free(table->name);
	free_linked_list<table_constraint_t>(table->constraints, [](table_constraint_t *data) {
		expression::free_exprnode(data->check_cond);
		free_column_ref(data->column_ref);
		free_column_ref(data->foreign_column_ref);
		free(data);
	} );

	for(field_item_t *it = table->fields; it; )
	{
		field_item_t *tmp = it;
		free(it->name);
		expression::free_exprnode(it->default_value);
		it = it->next;
		free(tmp);
	}

	free((void*)table);
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
	dbms::get_instance()->show_database(db_name);
	free((void*)db_name);
}

void execute_drop_table(const char *table_name)
{
	dbms::get_instance()->drop_table(table_name);
	free((void*)table_name);
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
		free_linked_list<expr_node_t>(expr_list, expression::free_exprnode);
	} );
	free((void*)insert_info);
}

void execute_delete(const delete_info_t *delete_info)
{
	dbms::get_instance()->delete_rows(delete_info);
	free(delete_info->table);
	expression::free_exprnode(delete_info->where);
	free((void*)delete_info);
}

void execute_select(const select_info_t *select_info)
{
	dbms::get_instance()->select_rows(select_info);
	expression::free_exprnode(select_info->where);
	free_linked_list<expr_node_t>(select_info->exprs, expression::free_exprnode);
	free_linked_list<table_join_info_t>(select_info->tables, [](table_join_info_t *data) {
		free(data->table);
		if(data->join_table)
			free(data->join_table);
		if(data->alias)
			free(data->alias);
		expression::free_exprnode(data->cond);
		free(data);
	} );

	free((void*)select_info);
}

void execute_update(const update_info_t *update_info)
{
	dbms::get_instance()->update_rows(update_info);
	free(update_info->table);
	free_column_ref(update_info->column_ref);
	expression::free_exprnode(update_info->where);
	expression::free_exprnode(update_info->value);
	free((void*)update_info);
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

void execute_quit()
{
	dbms::get_instance()->close_database();
	printf("[exit] good bye!\n");
}


