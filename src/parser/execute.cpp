#include <stdio.h>
#include <stdlib.h>
#include "execute.h"
#include "../database/database.h"

void execute_create_table(const table_def_t *table)
{
	printf("[create] table name = %s\n", table->name);
	for(field_item_t *field = table->fields; field; field = field->next)
	{
		printf("  [field] name = %s, type = %d, width = %d, flags = %d\n",
				field->name, field->type, field->width, field->flags);
	}
}

void execute_create_database(const char *db_name)
{
	database db;
	db.create(db_name);
	db.close();
	free((char*)db_name);
}

void execute_use_database(const char *db_name)
{
	printf("[use] database name = %s\n", db_name);
}

void execute_drop_database(const char *db_name)
{
	printf("[drop] database name = %s\n", db_name);
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
	printf("[insert] table = %s\n", insert_info->table);
	for(linked_list_t *l = insert_info->values; l; l = l->next)
	{
		printf("  [item]\n");
		for(void *p = l->data; p; p = ((linked_list_t*)p)->next)
		{
			printf("    [expr] ");
			traverse_expr((const expr_node_t*)((linked_list_t*)p)->data);
			printf("\n");
		}
	}
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
	printf("[exit] good bye!\n");
}


